echo "Running unit tests:"

for i in $TESTS_ROOT/*_tests
do
    if test -f $i
    then
        if $VALGRIND ./$i 2>> tests/tests.log # pipe output from stderr (2)
        then
            echo $i PASS
        else
            echo "ERROR in test $i: here's tests/tests.log"
            echo "------"
            tail tests/tests.log
            exit 1
        fi
    fi
done

echo ""