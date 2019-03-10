#1/bin/bash

# Try to find a portable way of getting rid of
# any stray carriage returns
if which dos2unix ; then
    DOS2UNIX="dos2unix"
elif which fromdos ; then
    DOS2UNIX="fromdos"
else
    # This works on a GNU version of sed. I think this
    # will work in OSX as well, but don't have a machine
    # on which to test that. From reading the OSX docs,
    # it looks compatible.
    # The code \x0D is the ASCII code of carriage-return,
    # so it the regex should delete any CRs at the end of
    # a line (or anywhere in a line)
    DOS2UNIX="sed -e s/\x0D//g -"
    # Tested for combinations of:
    # - Ubuntu
    # - Cygwin
    # and inputs formats:
    # - LF
    # - CRLF
fi

#clean things up

echo "Cleaning the temporaries and outputs"
echo "========================================"
make clean
echo ""
rm -r test_deliverable/working

echo "Building from scratch"
echo "========================================"
make all
if [[ "$?" -ne 0 ]]; then
	echo "Build failed.";
	exit 1
fi
echo ""

#begin testbench
if [[ -z "$1" ]]; then 
    COMPILER=bin/c_compiler
else
    COMPILER=$1
fi

mkdir -p test_deliverable/working

PASSED=0
CHECKED=0

for DRIVER in test_deliverable/test_cases/*_driver.c ; do
    NAME=$(basename $DRIVER _driver.c)
    TEST_ID=test_deliverable/test_cases/$NAME.c
    echo "Test case $NAME"

    # Step 1: Compile ${NAME}.c using the compiler-under-test into MIPS assembly.
    $COMPILER -S test_deliverable/test_cases/$NAME.c -o test_deliverable/working/$NAME.s
    if [[ $? -ne 0 ]]; then
        >&2 echo "ERROR : Compiler-under-test generating wrong code"
        continue
    fi
    
    # Step 2: Compile ${NAME}_driver.c using GCC into a MIPS object file.
    mips-linux-gnu-gcc -mfp32 -o test_deliverable/working/$NAME.o -c test_deliverable/working/$NAME.s
    if [[ $? -ne 0 ]]; then
        >&2 echo "ERROR : Test-case is malformed, GCC couldn't compile program"
        continue
    fi
    
    
    # Link driver object and assembly into executable
    mips-linux-gnu-gcc -mfp32 -static test_deliverable/working/$NAME.s test_deliverable/working/$NAME_driver.o -o test_deliverable/working/$NAME_output
    if [[ $? -ne 0 ]]; then
        >&2 echo "ERROR : Linker and assembly not working"
        continue
    fi
    
    # Run the actual executable
    qemu-mips test_deliverable/working/${NAME}_output
    GOT_RESULT=$?
    if [[ $result -ne 0 ]]; then
        >&2 echo "ERROR : Testcase returned $GOT_RESULT, but expected 0."
    else
        echo "Passed $NAME"
        PASSED=$(( ${PASSED}+1 ));
    fi

    CHECKED=$(( ${CHECKED}+1 ));
    echo ""

done

echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo ""

RELEASE=$(lsb_release -d)
if [[ $? -ne 0 ]]; then
    echo ""
    echo "Warning: This appears not to be a Linux environment"
    echo "         Make sure you do a final run on a lab machine or an Ubuntu VM"
else
    grep -q "Ubuntu 16.04" <(echo $RELEASE)
    FOUND=$?

    if [[ $FOUND -ne 0 ]]; then
        echo ""
        echo "Warning: This appears not to be the target environment"
        echo "         Make sure you do a final run on a lab machine or an Ubuntu VM"
    fi
fi