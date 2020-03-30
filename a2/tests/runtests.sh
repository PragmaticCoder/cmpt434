# Name: Alvi AKbar
# NSID: ala273
# Student No: 11118887

echo "Running Unit Tests:"

for i in tests/*_tests; do
    if test -f $i; then
        if $VALGRIND ./$i 2>>tests/tests.log; then
            echo "$i PASS"
        else
            echo "$ERROR in test $i"
            echo "For more info, see tests.log"
            echo "------"
            tail tests/tests.log
            exit 1
        fi
    fi
done

echo ""
