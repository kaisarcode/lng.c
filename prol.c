/**
 * prol - Probable language detector
 * Summary: Single-file token-profile language detection tool.
 *
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROL_VERSION "0.1.0"
#define PROL_LANG_UNKNOWN ""
#define PROL_TOKEN_MAX 128

typedef struct {
    const char *code;
    const char *words;
} KcProlMap;

static const KcProlMap kc_prol_maps[] = {
    { "en", " a and are as at be by can code come commit do documentation down for from get go have he healthy how i image in is it make not of on or readme refactor script see server she table take that the they this to update was we what which who why will with you " },
    { "es", " abajo actualizar codigo como commit con de documentacion donde el ella ellos en era eran es fue fueron guion hacer imagen ir la las los no nosotros para pero poder por porque que querer quien refactorizar sano servidor si son tabla tu un venir ver y yo " },
    { "pt", " a as ao aos com da das de do dos e ela elas ele eles em entre era eram esta estas este estes eu ha mais mas me mes meu na nas no nos nossa nosso o os ou para perante por qual quando que quem se sem sua suas te tem teu tu um uma voce " },
    { "fr", " a au aux avec ce ces dans de des du elle elles en est et eu il ils je la le les leur lui ma mais me mes mon ne nos notre nous on ou par pas pour qu que qui sa se ses son sur ta te tes ton tu un une vous " },
    { "de", " als am an auch auf aus bei bin bis da das dass dein dem den der des dich dir die diese dieser dieses dir ein eine einem einen einer eines er es euch euer fur haben hat hatte ich ihm ihn ihnen ihr ihre im in ist ja mich mir mit nach nicht noch nun nur ob oder sein seine sich sie sind so um und uns von vor war was weiter wie wir zu zum zur " },
    { "it", " al alla alle allo agli ai anche c che chi ci come con da dal dalla dello dei del della delle dello di ed e al gli ha hai hanno ho i il in io la le lei li lo ma mi mio ne nel nella no noi non nostro o per perche piu quale quanto quella quelle quelli quello questa queste questi questo se sei si siamo siete su sul sulla ti tu tuo un una uno vi voi vostro " },
    { "nl", " aan al alles als altijd andere ben bij daar dan dat de der deze die dit doe doen door dus een eens en er hem het hier hij hoe hun ik in is ja je kan me meer men met mij mijn moet na naar niet niets nog nu of om onder ons ook op over te tot u uit van voor wat we wel wij zal ze zelf zich zij zijn zo zonder " },
    { "sv", " alla allt att av blev bli blir da de dem den denna deras dess dessa det detta dig din dina du dar e ej eller en ar ett for fran for ha hade han hans har henne hennes hon honom hur i icke ingen inom inte jag ja ju kan kom kommer kunna man med mej mig min mina mitt mot mycket ni nu nar nagon nagot nagra och om oss pa sa sin sina sitta sjallv skulle som till upp ut vad var vara vart vi vid vilja vilken vilket vilka " },
    { "da", " af alle andet at blive da de dem den denne der deres det dig din disse dog du efter eller en end er et for fra ham han har havde hende hendes her hun i ikke ind jeg jo kan kom komme man med meget men mig min noget nogle nu nar og om op os pa sa selv sig sin sine skal sige som thi til ud under var vi vil ville " },
    { "no", " alle at av bare begge ble bli blir da de deg dem den denne der deres det dette din disse du e er et for fra ha hadde han hans har henne hennes her hit hun hva hvem hver hvilken hvordan hvorfor i ikke inn ja jeg jo kan kom man mange med meg men min mot mye ned no noe noen og om opp oss over pa sa seg selv si siden sin sine skal skulle slik som til ut var vil " },
    { "pl", " aby ach acz aczkolwiek aj albo ale alez ani az bardzo bez bo bowiem by byc byl byla byli bynajmniej caly ci cie ciebie co cokolwiek cos czasem czasu czy czyli daleko dla dlaczego dlatego do dobrze dokad dosc dzis dzisiaj gdy gdyz gdzie gdzies go i ich ile im inna inne ja jak jakas jakis jako jednak je jego jej jemu jesli jest juz m mnie moj na " },
    { "tr", " ama ancak artik asil az bana bazen bazi belki ben beni benim bes bile bir bircok biri birkac birkez biz bize bizi bizim boyle bu buna bundan bura butun da daha dahi de defa degil demek di diger diye eger en evet falan gibi ha hep her hic iki ile in is iste iyi kac " },
    { NULL, NULL }
};

/**
 * Reports whether one word exists in a space-padded word string.
 * @param word  Null-terminated lowercase token to search.
 * @param words Space-padded string of candidate words.
 * @return int  1 if found, 0 otherwise.
 */
static int prol_word_in_map(const char *word, const char *words) {
    char padded[PROL_TOKEN_MAX + 2];
    size_t len = strlen(word);

    if (len >= PROL_TOKEN_MAX) {
        return 0;
    }
    padded[0] = ' ';
    memcpy(padded + 1, word, len);
    padded[len + 1] = ' ';
    padded[len + 2] = '\0';

    return strstr(words, padded) != NULL ? 1 : 0;
}

/**
 * Scores one text against a word map by counting matching tokens.
 * @param text  Null-terminated UTF-8 input string.
 * @param words Space-padded string of candidate words.
 * @return int  Total number of matched tokens.
 */
static int prol_score(const char *text, const char *words) {
    const char *p = text;
    char token[PROL_TOKEN_MAX];
    int pos = 0;
    int score = 0;

    for (;;) {
        unsigned char c = (unsigned char)*p;

        if (c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (pos > 0) {
                token[pos] = '\0';
                if (prol_word_in_map(token, words)) {
                    score++;
                }
                pos = 0;
            }
            if (c == '\0') {
                break;
            }
        } else if (c < 128 && isalpha(c)) {
            if (pos < PROL_TOKEN_MAX - 1) {
                token[pos++] = (char)tolower(c);
            }
        } else {
            if (pos > 0) {
                token[pos] = '\0';
                if (prol_word_in_map(token, words)) {
                    score++;
                }
                pos = 0;
            }
        }
        p++;
    }
    return score;
}

/**
 * Detects the dominant language of one input string.
 * @param text Null-terminated UTF-8 input string.
 * @return Null-terminated BCP-47 language code, or "und" when undetermined.
 */
const char *kc_prol_detect(const char *text) {
    const char *best_code = PROL_LANG_UNKNOWN;
    int best_score = 0;
    size_t i = 0;

    if (!text || text[0] == '\0') {
        return PROL_LANG_UNKNOWN;
    }
    for (i = 0; kc_prol_maps[i].code != NULL; i++) {
        int score = prol_score(text, kc_prol_maps[i].words);

        if (score > best_score) {
            best_score = score;
            best_code = kc_prol_maps[i].code;
        }
    }
    return best_code;
}

#ifndef PROL_NO_MAIN

/**
 * Prints the command help to stdout.
 * @return void
 */
static void prol_help(void) {
    printf("Usage:\n");
    printf("  prol [options] [text]\n\n");
    printf("Description:\n");
    printf("  Detect the language of text from arguments or standard input.\n\n");
    printf("Options:\n");
    printf("  --help, -h      Show help\n");
    printf("  --version, -v   Show version\n\n");
    printf("Examples:\n");
    printf("  echo \"hello world\" | prol\n");
    printf("  prol \"hola mundo\"\n");
}

/**
 * Reads the full stdin payload into a heap buffer.
 * @param out Receives the allocated null-terminated string on success.
 * @return int 0 on success, 1 on failure.
 */
static int prol_read_stdin(char **out) {
    char chunk[512];
    char *buffer = NULL;
    size_t used = 0;
    size_t capacity = 0;

    if (!out) {
        return 1;
    }
    for (;;) {
        int count = read(STDIN_FILENO, chunk, sizeof(chunk));

        if (count < 0) {
            free(buffer);
            return 1;
        }
        if (count == 0) {
            break;
        }
        if (used + (size_t)count + 1 > capacity) {
            char *next;

            capacity = capacity == 0 ? 512u : capacity * 2u;
            while (capacity < used + (size_t)count + 1) {
                capacity *= 2u;
            }
            next = (char *)realloc(buffer, capacity);
            if (!next) {
                free(buffer);
                return 1;
            }
            buffer = next;
        }
        memcpy(buffer + used, chunk, (size_t)count);
        used += (size_t)count;
        buffer[used] = '\0';
    }
    if (!buffer) {
        buffer = (char *)malloc(1u);
        if (!buffer) {
            return 1;
        }
        buffer[0] = '\0';
    }
    *out = buffer;
    return 0;
}

/**
 * Writes one null-terminated line to stdout followed by a newline.
 * @param text Null-terminated string to emit.
 * @return int 0 on success, 1 on failure.
 */
static int prol_write_line(const char *text) {
    size_t length = strlen(text);

    while (length > 0) {
        int written = write(STDOUT_FILENO, text, length);

        if (written <= 0) {
            return 1;
        }
        text += (size_t)written;
        length -= (size_t)written;
    }
    {
        int nl = write(STDOUT_FILENO, "\n", 1);

        if (nl <= 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Application entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Process status code.
 */
int main(int argc, char **argv) {
    char *input = NULL;
    const char *lang = NULL;
    int from_stdin = 1;

    if (argc >= 2) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printf("prol %s\n", PROL_VERSION);
            return 0;
        }
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            prol_help();
            return 0;
        }
        if (argv[1][0] == '-') {
            fprintf(stderr, "Error: Unknown option.\n\n");
            prol_help();
            return 1;
        }
        from_stdin = 0;
    }

    if (from_stdin) {
        if (prol_read_stdin(&input) != 0) {
            fprintf(stderr, "Error: Unable to read stdin.\n");
            free(input);
            return 1;
        }
        lang = kc_prol_detect(input);
        free(input);
    } else {
        lang = kc_prol_detect(argv[1]);
    }

    return prol_write_line(lang);
}

#endif
