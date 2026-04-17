/**
 * prol.h
 * Summary: Public API for prol language detection.
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PROL_H
#define PROL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Detection result.
 * @param code ISO-like language code.
 * @param score Normalized confidence score.
 * @return No return value.
 */
typedef struct {
    const char *code;
    double score;
} prol_result_t;

/**
 * Initializes internal language profiles.
 * @param none No parameters.
 * @return 0 on success, or a non-zero initialization error code on failure.
 */
int prol_init(void);

/**
 * Detects the best language match for the given text.
 * @param text Input text to analyze.
 * @return Best matching language code, or NULL when unavailable.
 */
const char *prol_detect(const char *text);

/**
 * Detects and ranks language matches for the given text.
 * @param text Input text to analyze.
 * @param out Caller-provided output buffer.
 * @param max_results Maximum number of results to write.
 * @param threshold Minimum score required for inclusion.
 * @return Number of results written to out.
 */
int prol_detect_top(
    const char *text,
    prol_result_t *out,
    int max_results,
    double threshold
);

#ifdef __cplusplus
}
#endif

#endif
