# lng.c - Language Detection Library

`lng.c` is a minimalist C library and CLI for detecting the language of the given text using internal language profiles. It is designed as a composable native primitive for the KaisarCode ecosystem.

---

## CLI

Detect the language of text provided as an argument or via standard input.

### Examples

Single language detection:

```bash
./bin/x86_64/linux/lng "Hello world"
```

Ranked detection with threshold and limit:

```bash
./bin/x86_64/linux/lng "Hello world" -l 3 -t 0.1
```

Standard input processing:

```bash
printf 'hola mundo' | ./bin/x86_64/linux/lng
```

---

### Parameters

| Flag | Description |
| :--- | :--- |
| `-t`, `--threshold <n>` | Minimum score threshold |
| `-l`, `--limit <n>` | Maximum number of results |
| `-h`, `--help` | Show help and usage |
| `-v`, `--version` | Show version |

---

### Output

Results are printed as the language code (for single match) or code and score (for ranked results):

```
en
```

```
en: 0.9500
es: 0.0400
```

---

## Public API

```c
#include "lng.h"

kc_lng_init();

const char *code = kc_lng_detect("Hello world");

kc_lng_result_t results[3];
int count = kc_lng_detect_top("Hello world", results, 3, 0.1);
```

---

## Lifecycle

- `kc_lng_init()` - initializes internal language profiles. This call is idempotent and thread-safe.
- `kc_lng_detect()` and `kc_lng_detect_top()` - perform language detection. Both call `kc_lng_init()` internally if not yet initialized. Once initialized, all state is read-only and safe for concurrent access.

---

## Build

```bash
make all
make x86_64/linux
make x86_64/windows
make i686/linux
make i686/windows
make aarch64/linux
make aarch64/android
make armv7/linux
make armv7/android
make armv7hf/linux
make riscv64/linux
make powerpc64le/linux
make mips/linux
make mipsel/linux
make mips64el/linux
make s390x/linux
make loongarch64/linux
make clean
```

Artifacts are generated under:

```
bin/{arch}/{platform}/
```

---

**Author:** KaisarCode

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
