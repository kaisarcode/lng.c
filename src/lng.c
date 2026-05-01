/**
 * lng.c
 * Summary: Thin CLI wrapper for liblng.
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "lng.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KC_LNG_TEXT_CAP   8192
#define KC_LNG_RESULT_CAP 32
#define KC_LNG_VERSION    "0.1.0"

/**
 * Reads text from standard input into a dynamically allocated buffer.
 * @param out_text Destination pointer for the allocated text.
 * @return 0 on success, or -1 on failure.
 */
static int kc_lng_read_stdin(char **out_text) {
    char *data = NULL;
    size_t length = 0;
    size_t capacity = 0;
    char chunk[4096];
    size_t n;

    if (!out_text) {
        return -1;
    }

    while ((n = fread(chunk, 1, sizeof(chunk), stdin)) > 0) {
        if (length + n + 1 > capacity) {
            size_t next_cap = capacity ? capacity * 2 : 4096;
            while (next_cap < length + n + 1) {
                next_cap *= 2;
            }
            char *next_data = (char *)realloc(data, next_cap);
            if (!next_data) {
                free(data);
                return -1;
            }
            data = next_data;
            capacity = next_cap;
        }
        memcpy(data + length, chunk, n);
        length += n;
    }

    if (ferror(stdin)) {
        free(data);
        return -1;
    }

    if (length == 0) {
        free(data);
        *out_text = NULL;
        return 0;
    }

    data[length] = '\0';
    *out_text = data;
    return 0;
}

/**
 * Parses one integer CLI value.
 * @param text Input text.
 * @param out Output integer pointer.
 * @return 1 on success, 0 on failure.
 */
static int kc_lng_parse_int(const char *text, int *out) {
    char *end;
    long value;

    if (!text || !out) {
        return 0;
    }

    errno = 0;
    value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0') {
        return 0;
    }

    if (value < -2147483647L - 1L || value > 2147483647L) {
        return 0;
    }

    *out = (int)value;
    return 1;
}

/**
 * Parses one floating-point CLI value.
 * @param text Input text.
 * @param out Output double pointer.
 * @return 1 on success, 0 on failure.
 */
static int kc_lng_parse_double(const char *text, double *out) {
    char *end;
    double value;

    if (!text || !out) {
        return 0;
    }

    errno = 0;
    value = strtod(text, &end);

    if (errno != 0 || end == text || *end != '\0') {
        return 0;
    }

    *out = value;
    return 1;
}

/**
 * Prints the compact command help.
 * @return No return value.
 */
static void kc_lng_help(void) {
    printf("Usage:\n");
    printf("  lng [options] [text]\n\n");
    printf("Options:\n");
    printf("  --threshold, -t <n>  Minimum score threshold\n");
    printf("  --limit, -l <n>      Maximum number of results\n");
    printf("  --help, -h           Show help\n");
    printf("  --version, -v        Show version\n\n");
    printf("Examples:\n");
    printf("  lng \"hello world\"\n");
    printf("  printf 'hola mundo' | lng -l 3\n");
}

/**
 * Prints the binary version.
 * @return No return value.
 */
static void kc_lng_version(void) {
    printf("lng %s\n", KC_LNG_VERSION);
}

/**
 * Prints one CLI error followed by help, then returns 1.
 * @param message Error text.
 * @return Always 1.
 */
static int kc_lng_fail_usage(const char *message) {
    fprintf(stderr, "Error: %s\n\n", message);
    kc_lng_help();
    return 1;
}

/**
 * Standalone entry point.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument vector.
 * @return Process exit status.
 */
int main(int argc, char **argv) {
    kc_lng_result_t results[KC_LNG_RESULT_CAP];
    const char *text;
    char *stdin_text;
    double threshold;
    int limit;
    int i;
    int written;

    text = NULL;
    stdin_text = NULL;
    threshold = 0.001;
    limit = 1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            kc_lng_help();
            return 0;
        }

        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            kc_lng_version();
            return 0;
        }

        if (strcmp(argv[i], "--threshold") == 0 || strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                return kc_lng_fail_usage("Missing value for --threshold.");
            }

            if (!kc_lng_parse_double(argv[i + 1], &threshold)) {
                return kc_lng_fail_usage("Invalid value for --threshold.");
            }

            i++;
            continue;
        }

        if (strcmp(argv[i], "--limit") == 0 || strcmp(argv[i], "-l") == 0) {
            if (i + 1 >= argc) {
                return kc_lng_fail_usage("Missing value for --limit.");
            }

            if (!kc_lng_parse_int(argv[i + 1], &limit)) {
                return kc_lng_fail_usage("Invalid value for --limit.");
            }

            i++;
            continue;
        }

        if (argv[i][0] == '-') {
            return kc_lng_fail_usage("Unknown argument.");
        }

        if (text != NULL) {
            return kc_lng_fail_usage("Too many positional arguments.");
        }

        text = argv[i];
    }

    if (!text) {
        if (kc_lng_read_stdin(&stdin_text) != 0) {
            return 1;
        }
        text = stdin_text;
    }

    if (!text || !*text) {
        free(stdin_text);
        return 0;
    }

    if (limit < 1) {
        limit = 1;
    }

    if (limit > KC_LNG_RESULT_CAP) {
        limit = KC_LNG_RESULT_CAP;
    }

    if (kc_lng_init() != KC_LNG_OK) {
        fprintf(stderr, "Error: initialization failed.\n");
        free(stdin_text);
        return 1;
    }

    written = kc_lng_detect_top(text, results, limit, threshold);

    for (i = 0; i < written; i++) {
        if (limit == 1) {
            printf("%s\n", results[i].code);
        } else {
            printf("%s: %.4f\n", results[i].code, results[i].score);
        }
    }

    free(stdin_text);
    return 0;
}
