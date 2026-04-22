# prol.c - Specialized Text Processing Library

A minimalist C library for specialized text processing and sequence handling.

---

## Quick Start

### Build
Requires a C compiler and CMake 3.14+.
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release
```
*The `prol` binary will be generated directly in the root directory.*

### Usage
```bash
./prol "Input text data"
```

---

## Features
- **High Throughput**: Optimized for processing large volumes of text.
- **Unified Build**: Single CMake workflow for all platforms.
- **Minimalist**: Focused, autonomous implementation following KCS.
- **Native Performance**: Leverages local CPU features.

---

## Public API
```c
#include "prol.h"

// Initialize context
kc_prol_t *ctx = kc_prol_open();

// Execute logic
kc_prol_exec(ctx, "Input data");

// Clean up
kc_prol_close(ctx);
```

---

**Author:** KaisarCode

**Email:** [kaisar@kaisarcode.com](mailto:kaisar@kaisarcode.com)

**Website:** https://kaisarcode.com

**License:** https://www.gnu.org/licenses/gpl-3.0.html

© 2026 KaisarCode
