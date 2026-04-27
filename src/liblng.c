/**
 * liblng.c
 * Summary: N-gram profile matcher for language detection.
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "lng.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#define KC_LNG_NG_SIZE   3
#define KC_LNG_MAX_GRAMS 2048
#define KC_LNG_MAX_LANGS 32

typedef struct {
    char gram[12];
    int count;
} kc_lng_gram_t;

typedef struct {
    const char *code;
    const char *seed;
    kc_lng_gram_t profile[KC_LNG_MAX_GRAMS];
    int profile_size;
    long total;
} kc_lng_lang_t;

typedef struct {
    const char *code;
    double score;
} kc_lng_rank_t;

#define KC_LNG_LANG(code_, seed_) \
    { \
        .code = code_, \
        .seed = seed_, \
        .profile = {{{0}, 0}}, \
        .profile_size = 0, \
        .total = 0 \
    }

static kc_lng_lang_t kc_lng_langs[KC_LNG_MAX_LANGS] = {
    KC_LNG_LANG("en", "the and are for that with this have from they which would there their about which into through across because between world hello morning everyone project matches short text quick brown fox jumps over lazy dog. how are you doing today? this is a robust test for english language detection. documentation is vital for understanding systems."),
    KC_LNG_LANG("es", "que el la de en que lo los un una por para como al su sus con del por sobre entre mucho después también siempre mundo hola buenos días todos ¿cómo estás? este proyecto compara texto corto el zorro marrón salta sobre el perro. esperanza y libertad para todos los pueblos. la programación es un arte que requiere paciencia."),
    KC_LNG_LANG("pt", "que o a do da de em um uma para com por mais se os as ao das dos pelo pela seu sua como entre muito depois mundo olá bom dia amigos todos este projeto compara texto corto rápido raposa marrom salta sobre cão. a língua portuguesa é maravilhosa. como você está hoje? espero que tudo esteja bem com você e sua família."),
    KC_LNG_LANG("fr", "le la les de des un une et est dans que qui pour pas plus ce sur avec au par se sont nous vous son sa ses monde bonjour tous ce projet compare texte court le renard brun saute par dessus chien paresseux. comment allez-vous? la france est un pays magnifique. la liberté est un droit fondamental pour chaque être humain."),
    KC_LNG_LANG("it", "il la lo i le gli un una e di che in per con si sono ma come nel della delle questo quello non piu mondo ciao buongiorno a tutti questo progetto confronta testi brevi. l'italia è un paese stupendo. spero che questa giornata sia fantastica per te. la pasta e la pizza sono famose in tutto il mondo."),
    KC_LNG_LANG("de", "der die das und ein eine am im in zu von mit für auf den dem nicht ist auch sich als nach vor bei durch welt hallo guten morgen alle dieses projekt vergleicht kurzen text der schnelle braune fuchs springt über den hund. wie geht es dir? deutschland ist bekannt für technik. alles hat ein ende, nur die wurst hat zwei."),
    KC_LNG_LANG("nl", "de het een en in is op met voor als aan door naar om over uit bij voor zijn was maar niet uit over door overal. hoe gaat het vandaag met u? nederland is een land van water en molens. deze software werkt heel goed."),
    KC_LNG_LANG("sv", "och i som att en ett med för på av till den de gör om har han hon det vid från skulle kunna vara alla. hur mår du idag? sverige är ett vackert land i norden. vi gillar att fika och njuta av naturen."),
    KC_LNG_LANG("da", "og i det at en til af for med på de som vi er han har ikke ved om fra men de da over efter her. hvad hedder du? danmark er et dejligt land. vi elsker at cykle på de flade veje. hvordan går det med dig?"),
    KC_LNG_LANG("no", "og i det at en til av for med på de som vi er han har ikke ved om fra men de da over etter her. norge er et land med mange fjorder og fjell. hvordan har du det i dag? vi er stolte av vår natur."),
    KC_LNG_LANG("pl", "w i na do że z o na za przy od po do by się jako który nie ale tak jak dla nich. cześć jak się masz? polska to kraj o bogatej historii. lubię jeść pierogi i spacerować po starym mieście."),
    KC_LNG_LANG("tr", "ve bir bu da de için çok ama en daha her kadar gibi olan olanlar ancak değil mi için mi bu ne o zaman. nasılsın kardeşim? türkiye çok güzel bir ülke. istanbul kıtalar arası bir köprüdür."),
    KC_LNG_LANG("id", "dan yang di ke untuk ada adalah dengan dan itu ini saya kamu dia mereka kami kita dapat dalam dari pada. apa kabar hari ini teman-teman? indonesia adalah negara kepulauan yang sangat luas. saya suka makan nasi goreng."),
    KC_LNG_LANG("ro", "și în o de la care pe un pentru că să se a fi cu din s-au fost mai au prin pre ea el ei. ce mai faci? românia este o țară frumoasă situată în europa de est. mămăliga este un preparat tradițional foarte gustos."),
    KC_LNG_LANG("cs", "a že v na s do za pro k s o po u by jako který on ona oni ono jak se mi tebe. ahoj jak se máš? česká republika je známá svým pivem a hrady. praha je srdce evropy."),
    KC_LNG_LANG("hu", "a az és egy hogy van volt lesz neki nem ő de mint is ha már csak még el ki le be fel át. hogy vagy barátom? magyarország a gulyás és a paprika földje. budapest gyönyörű város a duna partján."),
    KC_LNG_LANG("fi", "ja se on että hänet hän he heidän meidän teidän olla oli on se että ei mutta niitä. mitä kuuluu? suomi on tuhansien järvien maa. revontulet ovat upeita talvella."),
    KC_LNG_LANG("ru", "и в во не на я что тот быть с а весь по он она они это как но так за из о от около привет как дела? доброе утро всем. русский язык сложный но интересный."),
    KC_LNG_LANG("uk", "і в на що та як він це не було за до для від про po але було при. як справи? україна — це вільна та незалежна країна. слава україні!"),
    KC_LNG_LANG("bg", "и в на че да са за той като се по от му си със бил здравейте как сте? българия е стара страна в европа. морето е красиво и планините са величествени. българският език има богата история."),
    KC_LNG_LANG("el", "και το να είναι στο με για του ότι δεν θα από με τα οι που την ο στην από. γεια σας τι κάνετε; η ελλάδα είναι η χώρα του φωτός και της δημοκρατίας. καλή σου μέρα."),
    KC_LNG_LANG("ar", "من في على أن إلى ما لا عن مع كان هو الذي التي هذا هذه كل بعد إذا كان. مرحبا بك يا صديقي كيف حالك؟ اللغة العربية لغة الضاد وهي لغة تاريخية عريقة."),
    KC_LNG_LANG("he", "את של על כי המה עם כל גם את זה פה אבל לא אם הוא היא הם אלו. מה שלומך היום? ישראל היא מדינה קטנה עם היסטוריה גדולה."),
    KC_LNG_LANG("hi", "और के में है कि को ही से का पर भी यह तो था वह वे जो किया जाता है। नमस्ते क्या हाल है? भारत एक बहुत बड़ा और विविधतापूर्ण देश है।"),
    KC_LNG_LANG("ja", "の に は を た で が と し て い れ ば な から まで より も ます です こんにちは。 日本は技術と伝統が共存する素晴らしい国です。"),
    KC_LNG_LANG("ko", "안녕하세요 감사합니다 이것은 언어 감지 프로젝트입니다. 한국어는 매우 아름다운 언어입니다. 오늘 기분이 어떠신가요?"),
    {
        .code = NULL,
        .seed = NULL,
        .profile = {{{0}, 0}},
        .profile_size = 0,
        .total = 0
    }
};

#ifdef _WIN32
static INIT_ONCE kc_lng_once = INIT_ONCE_STATIC_INIT;
#else
static pthread_once_t kc_lng_once = PTHREAD_ONCE_INIT;
#endif

/**
 * Returns the byte length of a UTF-8 sequence from its lead byte.
 * @param c Lead byte of the UTF-8 sequence.
 * @return Number of bytes in the sequence, or 1 for invalid input.
 */
static int kc_lng_u8_len(unsigned char c) {
    if (c < 0x80U) {
        return 1;
    }
    if ((c & 0xE0U) == 0xC0U) {
        return 2;
    }
    if ((c & 0xF0U) == 0xE0U) {
        return 3;
    }
    if ((c & 0xF8U) == 0xF0U) {
        return 4;
    }
    return 1;
}

/**
 * Performs basic UTF-8 lowercase folding for Cyrillic and Greek.
 * @param c1 First byte of the UTF-8 sequence (modified in place).
 * @param c2 Second byte of the UTF-8 sequence (modified in place).
 * @return No return value.
 */
static void kc_lng_u8_lower(unsigned char *c1, unsigned char *c2) {
    if (*c1 == 0xD0U && (*c2 >= 0x90U && *c2 <= 0xAFU)) {
        *c2 = (unsigned char)(*c2 + 0x20U);
    } else if (*c1 == 0xCEU && (*c2 >= 0x91U && *c2 <= 0xABU)) {
        *c2 = (unsigned char)(*c2 + 0x20U);
    }
}

/**
 * Normalizes input text by lowercasing and collapsing whitespace/punctuation.
 * @param input Input text to normalize.
 * @return Allocated string the caller must free, or NULL on error.
 */
static char *kc_lng_normalize(const char *input) {
    size_t i;
    size_t j;
    size_t len;
    int in_space;
    char *out;

    if (input == NULL) {
        return NULL;
    }

    len = strlen(input);
    out = (char *)malloc(len + 1U);
    if (out == NULL) {
        return NULL;
    }

    i = 0U;
    j = 0U;
    in_space = 1;
    while (i < len) {
        unsigned char c1;
        int clen;

        c1 = (unsigned char)input[i];
        clen = kc_lng_u8_len(c1);

        if (clen == 1 && (isspace((int)c1) || ispunct((int)c1))) {
            if (!in_space) {
                out[j++] = ' ';
                in_space = 1;
            }
            i++;
            continue;
        }

        if (clen == 1) {
            if (c1 >= 'A' && c1 <= 'Z') {
                out[j++] = (char)(c1 + ('a' - 'A'));
            } else {
                out[j++] = (char)c1;
            }
        } else if (clen == 2 && i + 1U < len) {
            unsigned char c2;

            c2 = (unsigned char)input[i + 1U];
            kc_lng_u8_lower(&c1, &c2);
            out[j++] = (char)c1;
            out[j++] = (char)c2;
        } else {
            int k;

            for (k = 0; k < clen && i + (size_t)k < len; k++) {
                out[j++] = input[i + (size_t)k];
            }
        }

        in_space = 0;
        i += (size_t)clen;
    }

    if (j > 0U && out[j - 1U] == ' ') {
        j--;
    }

    out[j] = '\0';
    return out;
}

/**
 * Builds an n-gram frequency profile from a language seed string.
 * @param lang Language entry to populate.
 * @return No return value.
 */
static void kc_lng_train(kc_lng_lang_t *lang) {
    char *normalized;
    int len;
    int i;

    if (lang == NULL || lang->profile_size > 0 || lang->seed == NULL) {
        return;
    }

    normalized = kc_lng_normalize(lang->seed);
    if (normalized == NULL) {
        return;
    }

    len = (int)strlen(normalized);
    for (
        i = 0;
        i <= len - KC_LNG_NG_SIZE;
        i += kc_lng_u8_len((unsigned char)normalized[i])
    ) {
        char gram[12];
        const char *it;
        int byte_count;
        int char_count;
        int j;
        int found;

        memset(gram, 0, sizeof(gram));
        it = normalized + i;
        byte_count = 0;
        char_count = 0;

        while (char_count < KC_LNG_NG_SIZE && (it - normalized) < len) {
            int clen;

            clen = kc_lng_u8_len((unsigned char)*it);
            if (byte_count + clen >= (int)sizeof(gram)) {
                break;
            }

            memcpy(gram + byte_count, it, (size_t)clen);
            byte_count += clen;
            it += clen;
            char_count++;
        }

        if (char_count != KC_LNG_NG_SIZE) {
            continue;
        }

        found = 0;
        for (j = 0; j < lang->profile_size; j++) {
            if (strcmp(lang->profile[j].gram, gram) == 0) {
                lang->profile[j].count++;
                found = 1;
                break;
            }
        }

        if (!found && lang->profile_size < KC_LNG_MAX_GRAMS) {
            strcpy(lang->profile[lang->profile_size].gram, gram);
            lang->profile[lang->profile_size].count = 1;
            lang->profile_size++;
        }

        lang->total++;
    }

    free(normalized);
}

/**
 * Computes a normalized match score for text against one language profile.
 * @param text Input text to evaluate.
 * @param lang Language profile to score against.
 * @return Score in [0, 1], or 0.0 when the match ratio is below threshold.
 */
static double kc_lng_score(const char *text, const kc_lng_lang_t *lang) {
    char *normalized;
    int len;
    int i;
    int total_grams;
    int matches;
    double log_sum;

    normalized = kc_lng_normalize(text);
    if (normalized == NULL || lang == NULL || lang->total <= 0L) {
        free(normalized);
        return 0.0;
    }

    len = (int)strlen(normalized);
    total_grams = 0;
    matches = 0;
    log_sum = 0.0;

    for (
        i = 0;
        i <= len - KC_LNG_NG_SIZE;
        i += kc_lng_u8_len((unsigned char)normalized[i])
    ) {
        char gram[12];
        const char *it;
        int byte_count;
        int char_count;
        int j;
        int count;

        memset(gram, 0, sizeof(gram));
        it = normalized + i;
        byte_count = 0;
        char_count = 0;

        while (char_count < KC_LNG_NG_SIZE && (it - normalized) < len) {
            int clen;

            clen = kc_lng_u8_len((unsigned char)*it);
            if (byte_count + clen >= (int)sizeof(gram)) {
                break;
            }

            memcpy(gram + byte_count, it, (size_t)clen);
            byte_count += clen;
            it += clen;
            char_count++;
        }

        if (char_count != KC_LNG_NG_SIZE) {
            continue;
        }

        count = 0;
        for (j = 0; j < lang->profile_size; j++) {
            if (strcmp(lang->profile[j].gram, gram) == 0) {
                count = lang->profile[j].count;
                matches++;
                break;
            }
        }

        log_sum += log(
            ((double)count + 0.1) /
            ((double)lang->total + ((double)lang->profile_size * 0.1))
        );
        total_grams++;
    }

    free(normalized);

    if (total_grams == 0 || ((double)matches / (double)total_grams) < 0.05) {
        return 0.0;
    }

    return 1.0 / (
        1.0 + exp(-8.0 * ((log_sum / (double)total_grams) - (-5.25)))
    );
}

/**
 * Compares two ranked results by descending score for qsort.
 * @param left Pointer to the first kc_lng_rank_t.
 * @param right Pointer to the second kc_lng_rank_t.
 * @return Negative, zero, or positive ordering value.
 */
static int kc_lng_rank_cmp(const void *left, const void *right) {
    const kc_lng_rank_t *a;
    const kc_lng_rank_t *b;

    a = (const kc_lng_rank_t *)left;
    b = (const kc_lng_rank_t *)right;

    if (b->score > a->score) {
        return 1;
    }
    if (b->score < a->score) {
        return -1;
    }
    return 0;
}

/**
 * Trains all language profiles exactly once.
 * @return No return value.
 */
static void kc_lng_init_once(void) {
    int i;

    for (i = 0; i < KC_LNG_MAX_LANGS && kc_lng_langs[i].code != NULL; i++) {
        kc_lng_train(&kc_lng_langs[i]);
    }
}

#ifdef _WIN32
/**
 * Windows one-time init trampoline for InitOnceExecuteOnce.
 * @param once One-time initialization object (unused).
 * @param param Unused callback parameter.
 * @param context Unused callback context.
 * @return TRUE on success.
 */
static BOOL CALLBACK kc_lng_init_once_win(
    PINIT_ONCE once,
    PVOID param,
    PVOID *context
) {
    (void)once;
    (void)param;
    (void)context;
    kc_lng_init_once();
    return TRUE;
}
#endif

/**
 * Initializes internal language profiles via once-control.
 * @return KC_LNG_OK on success, KC_LNG_EINIT on failure.
 */
int kc_lng_init(void) {
#ifdef _WIN32
    if (InitOnceExecuteOnce(&kc_lng_once, kc_lng_init_once_win, NULL, NULL)) {
        return KC_LNG_OK;
    }
    return KC_LNG_EINIT;
#else
    if (pthread_once(&kc_lng_once, kc_lng_init_once) != 0) {
        return KC_LNG_EINIT;
    }
    return KC_LNG_OK;
#endif
}

/**
 * Detects the best matching language for input text.
 * @param text Input text to analyze.
 * @return Best matching language code, or NULL when no match is found.
 */
const char *kc_lng_detect(const char *text) {
    kc_lng_result_t result;

    if (kc_lng_detect_top(text, &result, 1, 0.001) != 1) {
        return NULL;
    }

    return result.code;
}

/**
 * Detects top matching languages for input text.
 * @param text Input text to analyze.
 * @param out Caller-provided output buffer for results.
 * @param max_results Maximum number of results to write into out.
 * @param threshold Minimum score required for a result to be included.
 * @return Number of results written to out.
 */
int kc_lng_detect_top(
    const char *text,
    kc_lng_result_t *out,
    int max_results,
    double threshold
) {
    kc_lng_rank_t ranks[KC_LNG_MAX_LANGS];
    int lang_count;
    int i;
    int written;

    if (text == NULL || out == NULL || max_results <= 0) {
        return 0;
    }

    if (kc_lng_init() != KC_LNG_OK) {
        return 0;
    }

    lang_count = 0;
    for (i = 0; i < KC_LNG_MAX_LANGS && kc_lng_langs[i].code != NULL; i++) {
        ranks[lang_count].code = kc_lng_langs[i].code;
        ranks[lang_count].score = kc_lng_score(text, &kc_lng_langs[i]);
        lang_count++;
    }

    qsort(ranks, (size_t)lang_count, sizeof(ranks[0]), kc_lng_rank_cmp);

    written = 0;
    for (i = 0; i < lang_count && written < max_results; i++) {
        if (ranks[i].score >= threshold) {
            out[written].code = ranks[i].code;
            out[written].score = ranks[i].score;
            written++;
        }
    }

    return written;
}
