echo "Running unit tests:"

for i in tests/*_tests
do
    if test -f $i
    then
        if ./$i 2>> tests/temp/tests.log # pipe output from stderr (2)
        then
            echo $i PASS
        else
            echo "ERROR in test $i: here's tests/tests.log"
            echo "------"
            tail tests/temp/tests.log
            exit 1
        fi
    fi
done

echo ""