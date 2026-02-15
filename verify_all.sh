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
echo -e "Line1\nKey\nLine3" > test1.txt
echo -e "Key\nLine2" > test2.txt
# Expected: Line 2 in test1, Line 1 in test2.
# grep -n Key test1 test2
# test1:2:Key
# test2:1:Key
# cut -d: -f1 -> 2, 1
# sort -> 1, 2
./q3 "Key" test1.txt test2.txt > q3_out.txt
if diff <(echo -e "1\n2") q3_out.txt; then
    echo "Q3 passed basic test."
else
    echo "Q3 failed. Output:"
    cat q3_out.txt
fi

echo "--- Checking Binaries ---"
ls -l q1 q2 q3 q4
