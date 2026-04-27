#!/bin/bash
# test.sh
# Summary: Validation suite for lng covering all 26 supported languages.
# Author:  KaisarCode
# Website: https://kaisarcode.com
# License: https://www.gnu.org/licenses/gpl-3.0.html

# Prints one failure line.
# @param 1 Failure message.
# @return 1 on failure.
kc_test_fail() {
    printf '\033[31m[FAIL]\033[0m %s\n' "$1"
    return 1
}

# Prints one success line.
# @param 1 Success message.
# @return 0 on success.
kc_test_pass() {
    printf '\033[32m[PASS]\033[0m %s\n' "$1"
}

# Verifies the binary exists and is executable.
# @return 0 on success, 1 on failure.
kc_test_check_binary() {
    if [ ! -x "./bin/x86_64/linux/lng" ]; then
        return 1
    fi
    return 0
}

# Runs a single detection test.
# @param 1 Text to detect.
# @param 2 Expected language code.
# @return 0 on success, 1 on failure.
kc_test_run_case() {
    local text="$1"
    local expected="$2"
    local result

    result=$(printf '%s' "$text" | "$BIN")

    if [ "$result" = "$expected" ]; then
        kc_test_pass "[$expected] '$text'"
        return 0
    fi
    kc_test_fail "[$expected] '$text' -> got '$result'"
}

# Runs the complete validation suite for all 26 languages.
# @return 0 on success, 1 on failure.
kc_test_main() {
    local failed=0

    kc_test_check_binary || exit 1

    BIN="./bin/x86_64/linux/lng"

    kc_test_run_case "Hello world" "en" || failed=$((failed + 1))
    kc_test_run_case "Guten Morgen" "de" || failed=$((failed + 1))
    kc_test_run_case "Hoe gaat het?" "nl" || failed=$((failed + 1))
    kc_test_run_case "¿Cómo estás?" "es" || failed=$((failed + 1))
    kc_test_run_case "Bom dia amigos" "pt" || failed=$((failed + 1))
    kc_test_run_case "Bonjour tout le monde" "fr" || failed=$((failed + 1))
    kc_test_run_case "Ciao a tutti" "it" || failed=$((failed + 1))
    kc_test_run_case "Ce mai faci?" "ro" || failed=$((failed + 1))
    kc_test_run_case "Hur mår du?" "sv" || failed=$((failed + 1))
    kc_test_run_case "Hvordan går det?" "da" || failed=$((failed + 1))
    kc_test_run_case "Hvordan har du det?" "no" || failed=$((failed + 1))
    kc_test_run_case "Cześć, jak się masz?" "pl" || failed=$((failed + 1))
    kc_test_run_case "Jak se máš?" "cs" || failed=$((failed + 1))
    kc_test_run_case "привіт як справи" "uk" || failed=$((failed + 1))
    kc_test_run_case "доброе утро" "ru" || failed=$((failed + 1))
    kc_test_run_case "здравейте как сте" "bg" || failed=$((failed + 1))
    kc_test_run_case "Hogy vagy?" "hu" || failed=$((failed + 1))
    kc_test_run_case "Mitä kuuluu?" "fi" || failed=$((failed + 1))
    kc_test_run_case "γεια σας" "el" || failed=$((failed + 1))
    kc_test_run_case "Nasılsın?" "tr" || failed=$((failed + 1))
    kc_test_run_case "Apa kabar?" "id" || failed=$((failed + 1))
    kc_test_run_case "مرحبا بك" "ar" || failed=$((failed + 1))
    kc_test_run_case "מה שלומך?" "he" || failed=$((failed + 1))
    kc_test_run_case "नमस्ते" "hi" || failed=$((failed + 1))
    kc_test_run_case "こんにちは" "ja" || failed=$((failed + 1))
    kc_test_run_case "안녕하세요" "ko" || failed=$((failed + 1))

    if [ "$failed" -eq 0 ]; then
        return 0
    fi

    return 1
}

kc_test_main
