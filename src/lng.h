/**
 * lng.h
 * Summary: Public API for lng language detection.
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef KC_LNG_H
#define KC_LNG_H

#ifdef __cplusplus
extern "C" {
#endif

#define KC_LNG_OK    0
#define KC_LNG_EINIT 1
#define KC_LNG_EINVAL 2

/**
 * Detection result.
 * @param code ISO-like language code.
 * @param score Normalized confidence score in [0, 1].
 */
typedef struct {
    const char *code;
    double score;
} kc_lng_result_t;

/**
 * Initializes internal language profiles.
 * Idempotent and thread-safe via once-control.
 * @return KC_LNG_OK on success, KC_LNG_EINIT on failure.
 */
int kc_lng_init(void);

/**
 * Detects the best language match for the given text.
 * Calls kc_lng_init() internally if not yet initialized.
 * @param text Input text to analyze.
 * @return Best matching language code, or NULL when unavailable.
 */
const char *kc_lng_detect(const char *text);

/**
 * Detects and ranks language matches for the given text.
 * Calls kc_lng_init() internally if not yet initialized.
 * After initialization all state is read-only; safe for concurrent callers.
 * @param text Input text to analyze.
 * @param out Caller-provided output buffer.
 * @param max_results Maximum number of results to write into out.
 * @param threshold Minimum score required for inclusion.
 * @return Number of results written to out.
 */
int kc_lng_detect_top(
    const char *text,
    kc_lng_result_t *out,
    int max_results,
    double threshold
);

#ifdef __cplusplus
}
#endif

#endif
