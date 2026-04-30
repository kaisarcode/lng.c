# lng — Language Detector

`lng` is a minimalist language detector based on UTF-8 n-gram profiles. It identifies languages by comparing the n-gram frequency distribution of input text against pre-computed profiles for 26 supported languages.

---

## File Layout

```
lng.c/
├── src/
│   ├── lng.c          CLI entry point (main)
│   ├── liblng.c       Core library implementation
│   └── lng.h          Public API header
├── bin/               Compiled artifacts (committed, Git LFS)
│   ├── x86_64/{linux,windows}
│   ├── i686/{linux,windows}
│   ├── aarch64/{linux,android}
│   ├── armv7/{linux,android}
│   ├── armv7hf/linux
│   ├── riscv64/linux
│   ├── powerpc64le/linux
│   ├── mips/linux  mipsel/linux  mips64el/linux
│   ├── s390x/linux
│   └── loongarch64/linux
├── CMakeLists.txt
├── Makefile
├── test.sh
└── README.md
```

## Build

```bash
make all              # all 16 targets
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

Each target produces under `bin/{arch}/{platform}/`:
- `liblng.a` — static library
- `liblng.so` / `liblng.dll` / `liblng.dll.a` — shared library and import lib
- `lng` / `lng.exe` — CLI executable

## CLI

```bash
./bin/x86_64/linux/lng "This is an English sentence."
printf 'Este es un texto en español' | ./bin/x86_64/linux/lng -t 0.1
./bin/x86_64/linux/lng "Bonjour tout le monde" -l 3
```

**Options:**

- `--threshold`, `-t <n>`: Minimum score threshold (default: 0.001).
- `--limit`, `-l <n>`: Maximum number of results to display.
- `--help`, `-h`: Show help.
- `--version`, `-v`: Show version.

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

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
