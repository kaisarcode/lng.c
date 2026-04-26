# lng - Language Detector

`lng` is a high-performance, minimalist language detector based on UTF-8 n-gram profiles. It identifies languages by comparing the frequency distribution of n-grams in the input text against pre-computed profiles for dozens of supported languages.

---

## Quick Start

### Build
Requires a C compiler and CMake 3.14+.
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
*The `lng` binary will be generated directly in the root directory.*

### CLI Usage
```bash
lng [options] [text]
```

**Options:**
- `--threshold`, `-t <n>`: Minimum score threshold (default: 0.001).
- `--limit`, `-l <n>`: Maximum number of results to display.
- `--help`, `-h`: Show help.
- `--version`, `-v`: Show version.

**Examples:**
```bash
lng "This is an English sentence."
printf 'Este es un texto en español' | lng -t 0.1
```

---

## Public API

The library provides a simple, thread-safe API for language detection.

```c
#include "lng.h"

// 1. Initialize profiles (Optional, called internally by detection functions)
lng_init();

// 2. Detect best match
const char *lang = lng_detect("Hello world");

// 3. Detect top N matches
lng_result_t results[5];
int count = lng_detect_top("Some text to detect", results, 5, 0.01);
```

### Thread-Safety & Performance
- `lng_init()` is idempotent and thread-safe.
- `lng_detect()` and `lng_detect_top()` call `lng_init()` internally if it hasn't been initialized yet.
- After initialization, detection uses read-only language profiles, making it safe for concurrent use.
- **Note:** Caller-provided output buffers (like `lng_result_t` arrays) must not be shared unsafely between threads.

---

## Build Options

- `LNG_NATIVE`: Set to `ON` to enable `-march=native` optimizations (default: `OFF`).
    ```bash
    cmake -B build -DLNG_NATIVE=ON
    ```

---

**Author:** KaisarCode

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
