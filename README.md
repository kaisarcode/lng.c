# prol - Probable Language Detector

A lightweight, single-file C utility for probabilistic language detection.

## Features
- **Integrated Engine**: No external maps required. Supports 25 languages out of the box.
- **N-Gram Probabilities**: Uses 3-character N-gram sliding windows for high accuracy.
- **UTF-8 Aware**: Correctly handles accent marks and non-Latin charsets (Cyrillic, Greek, Hindi, etc.).
- **Zero-Match Protection**: Discards languages with no matching n-grams to avoid false positives.

## Supported Languages
`en`, `es`, `pt`, `fr`, `it`, `de`, `nl`, `sv`, `da`, `no`, `pl`, `tr`, `id`, `ro`, `cs`, `hu`, `fi`, `ru`, `uk`, `bg`, `el`, `ar`, `he`, `hi`, `ja`.

## Usage

### Simple detection
```bash
prol "¿Cómo estás?"
# Output: es
```

### Top Rank (Multi-result)
```bash
prol -l 3 "Hello world"
# Output: 
# en: 0.1878
# de: 0.0032
# nl: 0.0015
```

### Threshold filtering
```bash
prol -t 0.5 "Cześć!"
```

### Piped Input
```bash
echo "Bom dia" | prol
```

## Compilation
Standard C99. No dependencies.
```bash
gcc -O2 prol.c -o prol -lm
```

## License
KaisarCode Standard license.
