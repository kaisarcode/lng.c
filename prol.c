/**
 * prol - Probable language detector (v0.5.0)
 * 100% logic alignment with kc-tpm. Support for UTF-8 (Cyrillic, Greek, etc).
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

#define PROL_VERSION "0.5.0"
#define PROL_NG_SIZE 3
#define PROL_MAX_LANGS 32
#define PROL_MAX_PROF 2048

typedef struct { char g[12]; int n; } KcG;
typedef struct { const char *c; const char *s; KcG p[PROL_MAX_PROF]; int p_sz; long tot; } KcL;

static KcL ds[PROL_MAX_LANGS] = {
    {"en", "the and are for that with this have from they which would there their about which into through across because between world hello morning everyone project matches short text quick brown fox jumps over lazy dog. what is your name? this is a test and it should work perfectly well for most english texts."},
    {"es", "que el la de en que lo los un una por para como al su sus con del por sobre entre mucho después también siempre mundo hola buenos días todos ¿cómo estás? este proyecto compara texto corto el zorro marrón salta sobre perro perezoso. nuestra patria es grande."},
    {"pt", "que o a do da de em um uma para com por mais se os as ao das dos pelo pela seu sua como entre muito depois mundo olá bom dia todos este projeto compara texto curto rápido raposa marrom salta sobre cão preguiçoso. a língua portuguesa é bonita."},
    {"fr", "le la les de des un une et est dans que qui pour pas plus ce sur avec au par se sont nous vous son sa ses monde bonjour tous ce projet compare texte court le renard brun saute par dessus chien paresseux. comment allez-vous aujourd'hui?"},
    {"it", "il la lo i le gli un una e di che in per con si sono ma come nel della delle questo quello non piu mondo ciao buongiorno tutti questo progetto confronta testo corto la volpe marrone salta sopra cane pigro. come stai? il sole splende."},
    {"de", "der die das und ein eine am im in zu von mit für auf den dem nicht ist auch sich als nach vor bei durch welt hallo guten morgen alle dieses projekt vergleicht kurzen text der schnelle braune fuchs springt über faulen hund. wie geht es ihnen?"},
    {"ru", "и в во не на я что тот быть с а весь по он она они это как но так за из о от около привет как дела? доброе утро всем. этот проект сравнивает короткий текст с использованием профилей n-грамм. это легко и быстро. документация важна."},
    {"el", "και το να είναι στο με για του ότι δεν θα από με τα οι που την ο στην από. γεια σας τι κάνετε; καλημέρα σε όλους. αυτό το έργο συγκρίνει σύντομο κείμενο χρησιμοποιώντας προφίλ n-gram. είναι ελαφρύ και γρήγορο. η τεκμηρίωση είναι σημαντική."},
    {NULL, NULL, {{""}, 0}, 0, 0}
};

static int u8_len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

/* Basic UTF-8 lowercase for ASCII, Cyrillic and Greek */
static void u8_low(unsigned char *c1, unsigned char *c2) {
    if (*c1 == 0xD0 && (*c2 >= 0x90 && *c2 <= 0xAF)) *c2 += 0x20; /* Cyrillic A-П */
    else if (*c1 == 0xD0 && (*c2 >= 0xB0 && *c2 <= 0xBF)) ; /* Already low */
    else if (*c1 == 0xD1 && (*c2 >= 0x80 && *c2 <= 0x8F)) ; /* Already low */
    else if (*c1 == 0xCE && (*c2 >= 0x91 && *c2 <= 0xAB)) *c2 += 0x20; /* Greek Alpha-Omega */
}

static char* norm(const char *in) {
    if (!in) return NULL;
    size_t len = strlen(in);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    size_t j = 0; int in_s = 1;
    for (size_t i = 0; i < len; ) {
        unsigned char c1 = (unsigned char)in[i];
        int cl = u8_len(c1);
        if (cl == 1 && isspace(c1)) {
            if (!in_s) { out[j++] = ' '; in_s = 1; }
            i++;
        } else {
            if (cl == 1) {
                if (c1 >= 'A' && c1 <= 'Z') out[j++] = (char)(c1 + 32);
                else out[j++] = (char)c1;
            } else if (cl == 2) {
                unsigned char c2 = (unsigned char)in[i+1];
                u8_low(&c1, &c2);
                out[j++] = (char)c1; out[j++] = (char)c2;
            } else {
                for (int k = 0; k < cl; k++) out[j++] = in[i+k];
            }
            in_s = 0; i += cl;
        }
    }
    if (j > 0 && out[j-1] == ' ') j--;
    out[j] = '\0';
    return out;
}

static void train(KcL *l) {
    if (l->p_sz > 0) return;
    char *nt = norm(l->s); int len = strlen(nt);
    for (int i = 0; i <= len - PROL_NG_SIZE; i += u8_len((unsigned char)nt[i])) {
        char g[12] = {0}; const char *it = nt + i; int bc = 0, cc = 0;
        while (cc < PROL_NG_SIZE && (it - nt) < len) {
            int cl = u8_len((unsigned char)*it);
            if (bc + cl >= 12) break;
            memcpy(g + bc, it, cl); bc += cl; it += cl; cc++;
        }
        if (cc == PROL_NG_SIZE) {
            int f = 0;
            for (int j = 0; j < l->p_sz; j++)
                if (!strcmp(l->p[j].g, g)) { l->p[j].n++; f = 1; break; }
            if (!f && l->p_sz < PROL_MAX_PROF) {
                strcpy(l->p[l->p_sz].g, g); l->p[l->p_sz].n = 1; l->p_sz++;
            }
            l->tot++;
        }
    }
    free(nt);
}

static double calc_score(const char *txt, KcL *l) {
    char *nt = norm(txt);
    if (!nt) return 0.0;
    int len = strlen(nt);
    double ls = 0.0; int tg = 0;
    for (int i = 0; i <= len - PROL_NG_SIZE; i += u8_len((unsigned char)nt[i])) {
        char g[12] = {0}; const char *it = nt + i; int bc = 0, cc = 0;
        while (cc < PROL_NG_SIZE && (it - nt) < len) {
            int cl = u8_len((unsigned char)*it);
            if (bc + cl >= 12) break;
            memcpy(g + bc, it, cl); bc += cl; it += cl; cc++;
        }
        if (cc == PROL_NG_SIZE) {
            int n = 0;
            for (int j = 0; j < l->p_sz; j++)
                if (!strcmp(l->p[j].g, g)) { n = l->p[j].n; break; }
            ls += log(((double)n + 1.0) / ((double)l->tot + l->p_sz));
            tg++;
        }
    }
    free(nt);
    if (!tg) return 0.0;
    return 1.0 / (1.0 + exp(-8.0 * ((ls / tg) - (-5.25))));
}

typedef struct { const char *c; double s; } R;
static int cmp(const void *a, const void *b) {
    if (((R*)b)->s > ((R*)a)->s) return 1;
    return (((R*)b)->s < ((R*)a)->s) ? -1 : 0;
}

int main(int argc, char **argv) {
    double th = 0.001; int lim = 1; const char *txt = NULL; char buf[8192];
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-t") && i + 1 < argc) th = atof(argv[++i]);
        else if (!strcmp(argv[i], "-l") && i + 1 < argc) lim = atoi(argv[++i]);
        else if (argv[i][0] != '-') txt = argv[i];
    }
    if (!txt) { int n = fread(buf, 1, sizeof(buf) - 1, stdin); if (n > 0) { buf[n] = 0; txt = buf; } }
    if (!txt || !*txt) return 0;
    R r[PROL_MAX_LANGS]; int n = 0;
    while (ds[n].c) { train(&ds[n]); r[n].c = ds[n].c; r[n].s = calc_score(txt, &ds[n]); n++; }
    qsort(r, n, sizeof(R), cmp);
    for (int i = 0; i < n && i < lim; i++) {
        if (r[i].s >= th) {
            if (lim == 1) printf("%s\n", r[i].c);
            else printf("%s: %.4f\n", r[i].c, r[i].s);
        }
    }
    return 0;
}
