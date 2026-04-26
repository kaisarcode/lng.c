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

#define LNG_TEXT_CAP 8192
#define LNG_RESULT_CAP 32
#define LNG_VERSION "0.1.0"

/**
 * Reads text from standard input into the provided buffer.
 * @param buffer Destination buffer.
 * @param size Buffer size in bytes.
 * @return Pointer to buffer on success, or NULL on empty input.
 */
static const char *lng_read_stdin(char *buffer, size_t size) {
    size_t n;

    if (!buffer || size < 2) {
        return NULL;
    }

    n = fread(buffer, 1, size - 1, stdin);
    if (n == 0) {
        return NULL;
    }

    buffer[n] = '\0';
    return buffer;
}

/**
 * Parses one integer CLI value.
 * @param text Input text.
 * @param out Output integer pointer.
 * @return 1 on success, 0 on failure.
 */
static int lng_parse_int(const char *text, int *out) {
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
static int lng_parse_double(const char *text, double *out) {
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
static void lng_help(void) {
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
static void lng_version(void) {
    printf("lng %s\n", LNG_VERSION);
}

/**
 * Prints one CLI error followed by help.
 * @param message Error text.
 * @return Process exit status.
 */
static int lng_fail_usage(const char *message) {
    fprintf(stderr, "Error: %s\n\n", message);
    lng_help();
    return 1;
}

/**
 * Standalone entry point.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument vector.
 * @return Process exit status.
 */
int main(int argc, char **argv) {
    char buffer[LNG_TEXT_CAP];
    lng_result_t results[LNG_RESULT_CAP];
    const char *text;
    double threshold;
    int limit;
    int i;
    int written;

    text = NULL;
    threshold = 0.001;
    limit = 1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            lng_help();
            return 0;
        }

        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            lng_version();
            return 0;
        }

        if (strcmp(argv[i], "--threshold") == 0 || strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                return lng_fail_usage("Missing value for --threshold.");
            }

            if (!lng_parse_double(argv[i + 1], &threshold)) {
                return lng_fail_usage("Invalid value for --threshold.");
            }

            i++;
            continue;
        }

        if (strcmp(argv[i], "--limit") == 0 || strcmp(argv[i], "-l") == 0) {
            if (i + 1 >= argc) {
                return lng_fail_usage("Missing value for --limit.");
            }

            if (!lng_parse_int(argv[i + 1], &limit)) {
                return lng_fail_usage("Invalid value for --limit.");
            }

            i++;
            continue;
        }

        if (argv[i][0] == '-') {
            return lng_fail_usage("Unknown argument.");
        }

        if (text != NULL) {
            return lng_fail_usage("Too many positional arguments.");
        }

        text = argv[i];
    }

    if (!text) {
        text = lng_read_stdin(buffer, sizeof(buffer));
    }

    if (!text || !*text) {
        return 0;
    }

    if (limit < 1) {
        limit = 1;
    }

    if (limit > LNG_RESULT_CAP) {
        limit = LNG_RESULT_CAP;
    }

    if (lng_init() != 0) {
        fprintf(stderr, "Error: initialization failed.\n");
        return 1;
    }

    written = lng_detect_top(text, results, limit, threshold);

    for (i = 0; i < written; i++) {
        if (limit == 1) {
            printf("%s\n", results[i].code);
        } else {
            printf("%s: %.4f\n", results[i].code, results[i].score);
        }
    }

    return 0;
}
