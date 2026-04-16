# prol - Probable Language Detector

Single-file token-profile language detection primitive.

Detects the dominant language of a UTF-8 text payload using embedded word
frequency string maps. Compiled as a standalone native POSIX C executable
without external runtime dependencies.

## Usage

```bash
echo "hello world"   | prol
# en

prol "hola mundo"
# es

echo "???"           | prol
# 
```

## Parameter Reference

| Flag | Description |
| :--- | :--- |
| `--help`, `-h` | Show help and usage |
| `--version`, `-v` | Show binary version |

## Compilation

The codebase consists of a single C file. 

**Compile CLI (Native)**
```bash
gcc -std=c99 -Wall -Wextra -O2 prol.c -o prol
```

**Compile CLI (aarch64 Linux)**
```bash
aarch64-linux-gnu-gcc -std=c99 -Wall -Wextra -O2 prol.c -o prol
```

**Compile as Shared Library (`.so`)**
To embed `prol` into another application, disable the CLI and compile with `PIC`:
```bash
gcc -std=c99 -Wall -Wextra -O2 -fPIC -shared -DPROL_NO_MAIN prol.c -o libprol.so
```

When importing the library from another C app, simply declare the header yourself:
```c
const char *kc_prol_detect(const char *text);
```

## Supported Languages

| Code | Language |
| :--- | :--- |
| `en` | English |
| `es` | Spanish |
| `pt` | Portuguese |
| `fr` | French |
| `de` | German |
| `it` | Italian |
| `nl` | Dutch |
| `sv` | Swedish |
| `da` | Danish |
| `no` | Norwegian |
| `pl` | Polish |
| `tr` | Turkish |

---

**Author:** KaisarCode

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
