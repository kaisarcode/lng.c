/**
 * prol - Probable language detector (v0.6.5)
 * Summary: N-gram profile matcher for language detection.
 *
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

#define PROL_VERSION "0.6.5"
#define PROL_NG_SIZE 3
#define PROL_MAX_GRAMS 2048
#define PROL_MAX_LANGS 32

typedef struct {
    char g[12];
    int n;
} kc_prol_gram_t;

typedef struct {
    const char *c;
    const char *s;
    kc_prol_gram_t p[PROL_MAX_GRAMS];
    int p_sz;
    long tot;
} kc_prol_lang_t;

static kc_prol_lang_t kc_prol_ds[PROL_MAX_LANGS] = {
    {"en", "the and are for that with this have from they which would there their about which into through across because between world hello morning everyone project matches short text quick brown fox jumps over lazy dog."},
    {"es", "que el la de en que lo los un una por para como al su sus con del por sobre entre mucho después también siempre mundo hola buenos días todos ¿cómo estás? este proyecto compara texto corto el zorro marrón."},
    {"pt", "que o a do da de em um uma para com por mais se os as ao das dos pelo pela seu sua como entre muito depois mundo olá bom dia todos este projeto compara texto corto rápido raposa marrom salta sobre cão."},
    {"fr", "le la les de des un une et est dans que qui pour pas plus ce sur avec au par se sont nous vous son sa ses monde bonjour tous ce proyecto compare texte court le renard brun saute par dessus chien paresseux."},
    {"it", "il la lo i le gli un una e di che in per con si sono ma come nel della delle questo quello non piu mondo ciao buongiorno tutti questo progetto confronta testo corto la volpe marrone salta sopra cane pigro."},
    {"de", "der die das und ein eine am im in zu von mit für auf den dem nicht ist auch sich als nach vor bei durch welt hallo guten morgen alle dieses projekt vergleicht kurzen text der schnelle braune fuchs."},
    {"nl", "de het een en in is op met voor als aan door naar om over uit bij voor zijn was maar niet uit over door overal. hoe gaat het vandaag? deze software is zeer krachtig en lichtgewicht."},
    {"sv", "och i som att en ett med för på av till den de gör om har han hon det vid från skulle kunna vara alla. hur mår du idag? den snabba bruna räven hoppar över den lata hunden."},
    {"da", "og i det at en til af for med på de som vi er han har ikke ved om fra men de da over efter her. hvordan har du det? denne software er fantastisk og meget hurtig."},
    {"no", "og i det at en til av for med på de som vi er han har ikke ved om fra men de da over etter her. hvordan har du det i dag? dette prosjektet er veldig bra."},
    {"pl", "w i na do że z o na za przy od po do by się jako który nie ale tak jak dla nich. jak się masz? cześć wszystkim. dokumentacja jest bardzo ważna dla każdego projektu, ponieważ wyjaśnia, jak wszystko działa."},
    {"tr", "ve bir bu da de için çok ama en daha her kadar gibi olan olanlar ancak değil mi için mi bu ne o zaman. nasılsın? merhaba arkadaşlar. bu yazılım gerçekten harika."},
    {"id", "dan yang di ke untuk ada adalah dengan dan itu ini saya kamu dia mereka kami kita dapat dalam dari pada. apa kabar hari ini? proyek ini sangat bagus."},
    {"ro", "și în o de la care pe un pentru că să se a fi cu din s-au fost mai au prin pre ea el ei. ce mai faci astăzi? acest proiect este foarte bun."},
    {"cs", "a že v na s do za pro k s o po u by jako který on ona oni ono jak se mi tebe. jak se máš? toto je velmi zajímavý projekt."},
    {"hu", "a az és egy hogy van volt lesz neki nem ő de mint is ha már csak még el ki le be fel át. hogy vagy ma? ez egy nagyon jó projekt."},
    {"fi", "ja se on että hänet hän he heidän meidän teidän olla oli on se että ei mutta niitä. mitä kuuluu? tämä on hyvä hanke."},
    {"ru", "и в во не на я что тот быть с а весь по он она они это как но так за из о от около привет как дела? доброе утро всем. этот проект сравнивает короткий текст."},
    {"uk", "і в на що та як він це не було за до для від про по але було при. як справи? доброго ранку всім. цей проект порівнює короткий текст."},
    {"bg", "и в на че да са за той като се по от му си със са бил. здравейте как сте? този проект сравнява кратък текст."},
    {"el", "και το να είναι στο με για του ότι δεν θα από με τα οι που την ο στην από. γεια σας τι κάνετε; καλημέρα σε όλους. αυτό το έργο συγκρίνει κείμενο."},
    {"ar", "من في على أن إلى ما لا عن مع كان هو الذي التي هذا هذه كل بعد إذا كان. كيف حالك؟ مرحبا بك في هذا المشروع الرائع للجميع."},
    {"he", "את של על כי המה עם כל גם את זה פה אבל לא אם הוא היא הם אלו. מה שלומך? פרויקט נהדר לזיהוי שפות שונות."},
    {"hi", "और के में है कि को ही से का पर भी यह तो था वह वे जो किया जाता है। क्या हाल है? नमस्ते आपका स्वागत है इस सॉफ्टवेयर प्रोजेक्ट में।"},
    {"ko", "안녕하세요 감사합니다 이것은 언어 감지 프로젝트입니다. 한국어는 매우 아름답습니다. 모든 프로젝트에서 문서화는 중요합니다. 어떻게 지내세요?"},
    {"ja", "の に は を た で が と し て い れ ば な から まで より も ます です こんにちは。 プロジェクトは言語を非常に迅速に検出します。"},
    {NULL, NULL, {{""}, 0}, 0, 0}
};

/**
 * Returns the length in bytes of the next UTF-8 character.
 */
static int kc_prol_u8_len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

/**
 * Performs a basic UTF-8 case folding for common upstairs scripts.
 */
static void kc_prol_u8_low(unsigned char *c1, unsigned char *c2) {
    if (*c1 == 0xD0 && (*c2 >= 0x90 && *c2 <= 0xAF)) *c2 += 0x20;
    else if (*c1 == 0xCE && (*c2 >= 0x91 && *c2 <= 0xAB)) *c2 += 0x20;
}

/**
 * Normalizes input text following the kcs policy (collapse space, lowercase).
 * @return Allocated string that must be freed by caller.
 */
static char* kc_prol_norm(const char *in) {
    if (!in) return NULL;
    size_t len = strlen(in);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    size_t j = 0; int in_s = 1;
    for (size_t i = 0; i < len; ) {
        unsigned char c1 = (unsigned char)in[i];
        int cl = kc_prol_u8_len(c1);
        if (cl == 1 && isspace(c1)) {
            if (!in_s) { out[j++] = ' '; in_s = 1; }
            i++;
        } else {
            if (cl == 1) {
                if (c1 >= 'A' && c1 <= 'Z') out[j++] = (char)(c1 + 32);
                else out[j++] = (char)c1;
            } else if (cl == 2) {
                unsigned char c2 = (unsigned char)in[i+1];
                kc_prol_u8_low(&c1, &c2);
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

/**
 * Generates an N-gram profile from a language seed string.
 */
static void kc_prol_train(kc_prol_lang_t *l) {
    if (l->p_sz > 0) return;
    char *nt = kc_prol_norm(l->s); int len = strlen(nt);
    for (int i = 0; i <= len - PROL_NG_SIZE; i += kc_prol_u8_len((unsigned char)nt[i])) {
        char g[12] = {0}; const char *it = nt + i; int bc = 0, cc = 0;
        while (cc < PROL_NG_SIZE && (it - nt) < len) {
            int cl = kc_prol_u8_len((unsigned char)*it);
            if (bc + cl >= 12) break;
            memcpy(g + bc, it, cl); bc += cl; it += cl; cc++;
        }
        if (cc == PROL_NG_SIZE) {
            int f = 0;
            for (int j = 0; j < l->p_sz; j++)
                if (!strcmp(l->p[j].g, g)) { l->p[j].n++; f = 1; break; }
            if (!f && l->p_sz < PROL_MAX_GRAMS) {
                strcpy(l->p[l->p_sz].g, g); l->p[l->p_sz].n = 1; l->p_sz++;
            }
            l->tot++;
        }
    }
    free(nt);
}

/**
 * Calculates the log-likelihood score for a text against a language profile.
 * @return Normalized probability between 0 and 1.
 */
static double kc_prol_calculate(const char *txt, kc_prol_lang_t *l) {
    char *nt = kc_prol_norm(txt);
    if (!nt) return 0.0;
    int len = strlen(nt);
    double ls = 0.0; int tg = 0; int matches = 0;
    for (int i = 0; i <= len - PROL_NG_SIZE; i += kc_prol_u8_len((unsigned char)nt[i])) {
        char g[12] = {0}; const char *it = nt + i; int bc = 0, cc = 0;
        while (cc < PROL_NG_SIZE && (it - nt) < len) {
            int cl = kc_prol_u8_len((unsigned char)*it);
            if (bc + cl >= 12) break;
            memcpy(g + bc, it, cl); bc += cl; it += cl; cc++;
        }
        if (cc == PROL_NG_SIZE) {
            int n = 0;
            for (int j = 0; j < l->p_sz; j++)
                if (!strcmp(l->p[j].g, g)) { n = l->p[j].n; matches++; break; }
            ls += log(((double)n + 1.0) / ((double)l->tot + l->p_sz));
            tg++;
        }
    }
    free(nt);
    if (!tg || !matches) return 0.0;
    return 1.0 / (1.0 + exp(-8.0 * ((ls / tg) - (-5.25))));
}

typedef struct {
    const char *c;
    double s;
} kc_prol_res_t;

/**
 * Comparison function for sorting results by score.
 */
static int kc_prol_cmp(const void *a, const void *b) {
    if (((kc_prol_res_t*)b)->s > ((kc_prol_res_t*)a)->s) return 1;
    return (((kc_prol_res_t*)b)->s < ((kc_prol_res_t*)a)->s) ? -1 : 0;
}

/**
 * Standalone entry point.
 */
int main(int argc, char **argv) {
    double th = 0.001; int lim = 1; const char *txt = NULL; char buf[8192];
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-t") && i + 1 < argc) th = atof(argv[++i]);
        else if (!strcmp(argv[i], "-l") && i + 1 < argc) lim = atoi(argv[++i]);
        else if (argv[i][0] != '-') txt = argv[i];
    }
    if (!txt) { int n = fread(buf, 1, sizeof(buf) - 1, stdin); if (n > 0) { buf[n] = 0; txt = buf; } }
    if (!txt || !*txt) return 0;
    kc_prol_res_t r[PROL_MAX_LANGS]; int n = 0;
    while (n < PROL_MAX_LANGS && kc_prol_ds[n].c) {
        kc_prol_train(&kc_prol_ds[n]);
        r[n].c = kc_prol_ds[n].c;
        r[n].s = kc_prol_calculate(txt, &kc_prol_ds[n]);
        n++;
    }
    qsort(r, n, sizeof(kc_prol_res_t), kc_prol_cmp);
    for (int i = 0; i < n && i < lim; i++) {
        if (r[i].s >= th) {
            if (lim == 1) printf("%s\n", r[i].c);
            else printf("%s: %.4f\n", r[i].c, r[i].s);
        }
    }
    return 0;
}
