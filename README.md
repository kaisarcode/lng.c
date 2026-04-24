# prol - Probabilistic Language Detector

`prol` is a high-performance, minimalist probabilistic language detector based on UTF-8 n-gram profiles. It identifies languages by comparing the frequency distribution of n-grams in the input text against pre-computed profiles for dozens of supported languages.

---

## Quick Start

### Build
Requires a C compiler and CMake 3.14+.
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
*The `prol` binary will be generated directly in the root directory.*

### CLI Usage
```bash
prol [options] [text]
```

**Options:**
- `--threshold`, `-t <n>`: Minimum score threshold (default: 0.001).
- `--limit`, `-l <n>`: Maximum number of results to display.
- `--help`, `-h`: Show help.
- `--version`, `-v`: Show version.

**Examples:**
```bash
prol "This is an English sentence."
printf 'Este es un texto en español' | prol -t 0.1
```

---

## Public API

The library provides a simple, thread-safe API for language detection.

```c
#include "prol.h"

// 1. Initialize profiles (Optional, called internally by detection functions)
prol_init();

// 2. Detect best match
const char *lang = prol_detect("Hello world");

// 3. Detect top N matches
prol_result_t results[5];
int count = prol_detect_top("Some text to detect", results, 5, 0.01);
```

### Thread-Safety & Performance
- `prol_init()` is idempotent and thread-safe.
- `prol_detect()` and `prol_detect_top()` call `prol_init()` internally if it hasn't been initialized yet.
- After initialization, detection uses read-only language profiles, making it safe for concurrent use.
- **Note:** Caller-provided output buffers (like `prol_result_t` arrays) must not be shared unsafely between threads.

---

## Build Options

- `PROL_NATIVE`: Set to `ON` to enable `-march=native` optimizations (default: `OFF`).
  ```bash
  cmake -B build -DPROL_NATIVE=ON
  ```

---

**Author:** KaisarCode

**Website:** https://kaisarcode.com

**License:** https://www.gnu.org/licenses/gpl-3.0.html

© 2026 KaisarCode
