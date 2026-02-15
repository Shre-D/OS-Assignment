#!/bin/bash
echo "--- Verifying Q1 ---"
# Run Q1 with n=1 (should be instantaneous sleeps)
timeout 5s echo "1 8 3 15 4 6 7 17 9 2" | ./q1 > q1_out.txt
if [ $? -eq 0 ]; then
    echo "Q1 ran successfully."
    cat q1_out.txt | head -n 5
    echo "..."
else
    echo "Q1 failed or timed out."
fi

echo "--- Verifying Q3 ---"
# Test 1: Search "Hello" in both files
# Belgium.txt: lines 1, 3
# France.txt: lines 1, 2
# Expected lines: 1, 2, 3
./q3 "Hello" Belgium.txt France.txt > q3_out.txt
if diff <(echo -e "1\n2\n3") q3_out.txt; then
    echo "Q3 Test 1 (Hello) passed."
else
    echo "Q3 Test 1 (Hello) failed. Expected 1, 2, 3."
    cat q3_out.txt
fi

# Test 2: Search "Waffles" in Belgium.txt
# Belgium.txt: lines 2, 3
./q3 "Waffles" Belgium.txt > q3_out.txt
if diff <(echo -e "2\n3") q3_out.txt; then
    echo "Q3 Test 2 (Waffles) passed."
else
    echo "Q3 Test 2 (Waffles) failed. Expected 2, 3."
    cat q3_out.txt
fi

# Test 3: Search "Croissants" in France.txt
# France.txt: line 2
./q3 "Croissants" France.txt > q3_out.txt
if diff <(echo -e "2") q3_out.txt; then
    echo "Q3 Test 3 (Croissants) passed."
else
    echo "Q3 Test 3 (Croissants) failed. Expected 2."
    cat q3_out.txt
fi


echo "--- Verifying Q2 ---"
# Run Q2 with n=1, k=5, r=1.
# Input "1 5 1" starts it.
# Input "-2" tells the parent to exit when it gets a chance (after 1 second).
# We wait slightly to ensure it processes.
echo -e "1 5 1\n-2" | timeout 5s ./q2 > q2_out.txt
if [ $? -eq 0 ]; then
    echo "Q2 ran successfully."
    # Check if we got at least one memory report and the parent prompt
    if grep -q "PID" q2_out.txt && grep -q "Parent:" q2_out.txt; then
        echo "Q2 output looks valid."
    else
        echo "Q2 ran but output is missing expected strings."
    fi
else
    echo "Q2 failed or timed out."
    cat q2_out.txt
fi

echo "--- Verifying Q4 ---"
# Run Q4 with commands: get_freq, set_freq 900, get_freq, quit
echo -e "get_freq\nset_freq 900\nget_freq\nquit" | ./q4 > q4_out.txt
# Check for initial frequency 800 and updated frequency 900
if grep -q "Current Frequency: 800 MHz" q4_out.txt && grep -q "Current Frequency: 900 MHz" q4_out.txt; then
    echo "Q4 functional test passed (Frequency update verified)."
else
    echo "Q4 failed functional test."
    cat q4_out.txt
fi

echo "--- Checking Binaries ---"
ls -l q1 q2 q3 q4
