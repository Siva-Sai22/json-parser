#!/usr/bin/env bash

PARSER="./build/src/JSONParser"

passed=0
failed=0

for dir in tests/step*; do
    echo "== $(basename "$dir") =="

    if [[ -f "$dir/valid.json" ]]; then
        "$PARSER" "$dir/valid.json" >/dev/null 2>&1

        if [[ $? -eq 0 ]]; then
            echo "✓ valid.json"
            ((passed++))
        else
            echo "✗ valid.json"
            ((failed++))
        fi
    fi

    if [[ -f "$dir/invalid.json" ]]; then
        "$PARSER" "$dir/invalid.json" >/dev/null 2>&1

        if [[ $? -ne 0 ]]; then
            echo "✓ invalid.json"
            ((passed++))
        else
            echo "✗ invalid.json"
            ((failed++))
        fi
    fi

    echo
done

echo "Passed: $passed"
echo "Failed: $failed"

[[ $failed -eq 0 ]]