/**
 * prol - Probable language detector (v0.4.1)
 * 100% logic alignment with kc-tpm. Curated multilingual dataset.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

#define PROL_VERSION "0.4.1"
#define PROL_NG_SIZE 3
#define PROL_MAX_LANGS 32
#define PROL_MAX_PROF 2048

typedef struct { char g[12]; int n; } KcG;
typedef struct { const char *c; const char *s; KcG p[PROL_MAX_PROF]; int p_sz; long tot; } KcL;

static KcL ds[PROL_MAX_LANGS] = {
    {"en", "the and are for that with this have from they which would there their about which into through across because between world hello morning everyone project matches short text quick brown fox jumps over lazy dog. what is your name? this is a test and it should work perfectly well for most english texts. documentation is the most important part of any software project because it explains how things work."},
    {"es", "que el la de en que lo los un una por para como al su sus con del por sobre entre mucho después también siempre mundo hola buenos días todos ¿cómo estás? este proyecto compara texto corto el zorro marrón salta sobre perro perezoso. nuestra patria es grande y hermosa. la documentación es la parte más importante de cualquier proyecto de software porque explica cómo funcionan las cosas."},
    {"pt", "que o a do da de em um uma para com por mais se os as ao das dos pelo pela seu sua como entre muito depois mundo olá bom dia todos este projeto compara texto curto rápido raposa marrom salta sobre cão preguiçoso. a língua portuguesa é muito bonita e rica em história. você está bem hoje? a documentação é essencial para o sucesso de qualquer projeto de engenharia de software."},
    {"fr", "le la les de des un une et est dans que qui pour pas plus ce sur avec au par se sont nous vous son sa ses monde bonjour tous ce projet compare texte court le renard brun saute par dessus chien paresseux. comment allez-vous aujourd'hui? la documentation est la partie la plus importante de tout projet car elle explique comment les choses fonctionnent vraiment dans le système."},
    {"it", "il la lo i le gli un una e di che in per con si sono ma come nel della delle questo quello non piu mondo ciao buongiorno tutti questo progetto confronta testo corto la volpe marrone salta sopra cane pigro. come stai? il sole splende oggi. la documentazione è la chiave per il successo di ogni progetto software, poiché spiega in dettaglio come funziona l'intero sistema integrato."},
    {"de", "der die das und ein eine am im in zu von mit für auf den dem nicht ist auch sich als nach vor bei durch welt hallo guten morgen alle dieses projekt vergleicht kurzen text der schnelle braune fuchs springt über faulen hund. wie geht es ihnen heute? diese software ist wirklich großartig. dokumentation ist wichtig für jedes projekt, damit andere verstehen können wie alles funktioniert."},
    {"nl", "de het een en in is op met voor als aan door naar om over uit bij voor zijn was maar niet uit over door overal. hoe gaat het vandaag? deze software is zeer krachtig en lichtgewicht. documentatie is heel belangrijk voor elk project omdat het uitlegt hoe alles werkt in de praktijk."},
    {"sv", "och i som att en ett med för på av till den de gör om har han hon det vid från skulle kunna vara alla. hur mår du idag? den snabba bruna räven hoppar över den lata hunden. dokumentation är nyckeln till framgång för alla programvaruprojekt eftersom den förklarar hur systemet fungerar."},
    {"da", "og i det at en til af for med på de som vi er han har ikke ved om fra men de da over efter her. hvordan har du det? denne software er fantastisk og meget hurtig. dokumentation er vigtig for ethvert projekt, da det forklarer hvordan tingene fungerer."},
    {"no", "og i det at en til av for med på de som vi er han har ikke ved om fra men de da over etter her. hvordan har du det i dag? dette prosjektet er veldig bra. dokumentasjon er viktig for alle prosjekter fordi det forklarer hvordan systemet fungerer."},
    {"pl", "w i na do że z o na za przy od po do by się jako który nie ale tak jak dla nich. jak się masz? cześć wszystkim. dokumentacja jest bardzo ważna dla każdego projektu, ponieważ wyjaśnia, jak wszystko działa w praktyce. czerwony lis skacze nad leniwym psem."},
    {"tr", "ve bir bu da de için çok ama en daha her kadar gibi olan olanlar ancak değil mi için mi bu ne o zaman. nasılsın? merhaba arkadaşlar. bu yazılım gerçekten harika. dokümantasyon her proje için önemlidir çünkü her şeyin nasıl çalıştığını açıklar."},
    {"id", "dan yang di ke untuk ada adalah dengan dan itu ini saya kamu dia mereka kami kita dapat dalam dari pada. apa kabar hari ini? proyek ini sangat bagus. dokumentasi sangat penting untuk setiap proyek karena menjelaskan bagaimana semuanya bekerja."},
    {"ro", "și în o de la care pe un pentru că să se a fi cu din s-au fost mai au prin pre ea el ei. ce mai faci astăzi? acest proiect este foarte bun. documentația este esențială pentru succesul oricărui proiect, deoarece explică modul în care funcționează totul."},
    {"cs", "a že v na s do za pro k s o po u by jako který on ona oni ono jak se mi tebe. jak se máš? toto je velmi zajímavý projekt. dokumentace je důležitá pro každý projekt, protože vysvětluje, jak vše funguje."},
    {"hu", "a az és egy hogy van volt lesz neki nem ő de mint is ha már csak még el ki le be fel át. hogy vagy ma? ez egy nagyon jó projekt. a dokumentáció fontos minden projekt számára, mert elmagyarázza, hogyan működik minden."},
    {"fi", "ja se on että hänet hän he heidän meidän teidän olla oli on se että ei mutta niitä. mitä kuuluu? tämä on hyvä hanke. dokumentointi on erittäin tärkeää jokaiselle projektille, koska se selittää, miten kaikki toimii."},
    {NULL, NULL, {{""}}, 0, 0}
};

static int u8_len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

static char* norm(const char *in) {
    if (!in) return NULL;
    size_t len = strlen(in);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    size_t j = 0; int in_s = 1;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)in[i];
        if (isspace(c)) { if (!in_s) { out[j++] = ' '; in_s = 1; } }
        else { if (c >= 'A' && c <= 'Z') out[j++] = (char)(c + 32); else out[j++] = (char)c; in_s = 0; }
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
