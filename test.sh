#!/bin/bash
# test.sh
# Summary: Exhaustive validation suite for prol supporting all 26 languages.
# Author:  KaisarCode
# Website: https://kaisarcode.com
# License: https://www.gnu.org/licenses/gpl-3.0.html

# Prints one failure line using the shared KCS color style.
# @param 1 Failure message.
# @return 1 on failure.
kc_test_fail() {
    printf "\033[31m[FAIL]\033[0m %s\n" "$1"
    return 1
}

# Prints one success line using the shared KCS color style.
# @param 1 Success message.
# @return 0 on success.
kc_test_pass() {
    printf "\033[32m[PASS]\033[0m %s\n" "$1"
}

# Check if the prol binary exists
# @return 0 if exists, 1 otherwise
kc_test_check_binary() {
    if [ ! -f "./prol" ]; then
        echo "[ERROR] prol binary not found. Please compile first."
        return 1
    fi
    return 0
}

# Run a single detection test
# @param 1 text to detect
# @param 2 expected language code
# @return 0 on success, 1 on failure
kc_test_run_case() {
    local text="$1"
    local expected="$2"
    local result
    
    result=$(echo "$text" | ./prol)
    
    if [ "$result" = "$expected" ]; then
        kc_test_pass "[$expected] '$text'"
        return 0
    else
        kc_test_fail "[$expected] '$text' -> Got '$result'"
        return 1
    fi
}

# Run the complete test suite for all 26 languages
# @return 0 if all tests pass, 1 otherwise
kc_test_main() {
    local failed=0
    
    kc_test_check_binary || exit 1
    
    echo "Starting full prol validation suite (26 languages)..."
    echo "----------------------------------------------------"
    
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
    
    echo "----------------------------------------------------"
    if [ "$failed" -eq 0 ]; then
        echo "[SUCCESS] All 26 tests passed!"
        return 0
    else
        echo "[FAILURE] $failed tests failed."
        return 1
    fi
}

kc_test_main
