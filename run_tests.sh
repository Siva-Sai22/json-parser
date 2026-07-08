#!/bin/bash

EXECUTABLE="./build/src/json_parser"
TEST_DIR="./tests"

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

PASSED=0
FAILED=0
TOTAL=0

if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}Error: Executable not found at $EXECUTABLE${NC}"
    echo "Make sure you have built the project using CMake first."
    exit 1
fi

echo "Starting JSON Parser Tests..."
echo "------------------------------------------------"

for test_file in $(find "$TEST_DIR" -type f -name "*.json" | sort); do
    ((TOTAL++))

    "$EXECUTABLE" "$test_file" > /dev/null 2>&1
    EXIT_CODE=$?

    filename=$(basename -- "$test_file")

    if [[ "$filename" == valid* ]]; then
        if [ $EXIT_CODE -eq 0 ]; then
            echo -e "${GREEN}[PASS]${NC} $test_file"
            ((PASSED++))
        else
            echo -e "${RED}[FAIL]${NC} $test_file (Expected: Success, Got: Exit Code $EXIT_CODE)"
            ((FAILED++))
        fi

    elif [[ "$filename" == invalid* ]]; then
        if [ $EXIT_CODE -ne 0 ]; then
            echo -e "${GREEN}[PASS]${NC} $test_file"
            ((PASSED++))
        else
            echo -e "${RED}[FAIL]${NC} $test_file (Expected: Failure, Got: Exit Code 0)"
            ((FAILED++))
        fi

    else
        echo "[-] Skipping $test_file (Filename must start with 'valid' or 'invalid')"
        ((TOTAL--))
    fi
done

echo "------------------------------------------------"
echo -e "Test Summary: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC} out of $TOTAL total tests."

if [ $FAILED -ne 0 ]; then
    exit 1
else
    exit 0
fi
