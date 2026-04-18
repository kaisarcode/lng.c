# prol - Probable Language Detector

`prol` is a small C library and CLI for probabilistic language detection using embedded 3-gram profiles.

## What It Does

For an input text, `prol`:

1. normalizes the text with UTF-8 aware handling
2. scores it against embedded language profiles
3. ranks the most probable matches
4. returns either the best match or the top candidates

## Public API

The library is exposed through [prol.h](./prol.h).

Core types:

- `prol_result_t`

Core functions:

- `prol_init()`
- `prol_detect()`
- `prol_detect_top()`

`prol_detect_top()` writes ranked results into a caller-provided buffer and returns the number of entries written.

## CLI Usage

```bash
prol [options] [text]
```

Options:

- `--threshold`, `-t <n>`
- `--limit`, `-l <n>`
- `--help`, `-h`
- `--version`, `-v`

Examples:

```bash
prol "¿Cómo estás?"
prol -l 3 "Hello world"
prol -t 0.5 "Cześć!"
printf 'Bom dia' | prol
```

When `--limit` is `1`, `prol` prints only the best language code.
When `--limit` is greater than `1`, it prints `code: score` lines.

## Build

POSIX:

```bash
cc -O2 -std=c99 -I. libprol.c prol.c -lm -pthread -o prol
```

Windows:

```bash
cl /std:c11 /O2 libprol.c prol.c /I.
```

The source code is written to stay portable across Windows, macOS, iOS,
Linux, and Android. Final compiler flags and output names depend on the toolchain.

## Library Example

```c
#include "prol.h"

int main(void) {
    const char *lang;

    if (prol_init() != 0) {
        return 1;
    }

    lang = prol_detect("Bonjour tout le monde");
    return lang ? 0 : 1;
}
```

## Supported Languages

- English (`en`)
- Spanish (`es`)
- Portuguese (`pt`)
- French (`fr`)
- Italian (`it`)
- German (`de`)
- Dutch (`nl`)
- Swedish (`sv`)
- Danish (`da`)
- Norwegian (`no`)
- Polish (`pl`)
- Turkish (`tr`)
- Indonesian (`id`)
- Romanian (`ro`)
- Czech (`cs`)
- Hungarian (`hu`)
- Finnish (`fi`)
- Russian (`ru`)
- Ukrainian (`uk`)
- Bulgarian (`bg`)
- Greek (`el`)
- Arabic (`ar`)
- Hebrew (`he`)
- Hindi (`hi`)
- Japanese (`ja`)
- Korean (`ko`)

---

**Author:** KaisarCode

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
