# lng — Language Detector

`lng` is a minimalist language detector based on UTF-8 n-gram profiles. It identifies languages by comparing the n-gram frequency distribution of input text against pre-computed profiles for 26 supported languages.

---

## Quick Start

### Build

Requires CMake 3.14+ and a C11 compiler.

```bash
make x86_64/linux
```

Or build all 16 cross-platform targets at once:

```bash
make
```

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
lng "Bonjour tout le monde" -l 3
```

---

## Public API

```c
#include "lng.h"

kc_lng_init();

const char *lang = kc_lng_detect("Hello world");

kc_lng_result_t results[5];
int count = kc_lng_detect_top("Some text to detect", results, 5, 0.01);
```

### Threading

`kc_lng_init()` uses a once-control (`pthread_once` / `InitOnceExecuteOnce`) and is safe to call from multiple threads. After initialization all internal state is read-only, so `kc_lng_detect()` and `kc_lng_detect_top()` are safe for concurrent callers without additional locking. The caller is responsible for not sharing output buffers between threads.

---

## Build Options

- `LNG_NATIVE`: Enable `-march=native` (default: `OFF`).

```bash
cmake -B .build -DLNG_NATIVE=ON
```

---

**Author:** KaisarCode

**Email:** kaisarcode@gmail.com

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
