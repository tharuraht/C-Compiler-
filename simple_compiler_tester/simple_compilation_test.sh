#!/bin/bash
bin/c_compiler -S simple_compiler_tester/test_program.c -o simple_compiler_tester/test_program.s
mips-linux-gnu-gcc -mfp32 -o simple_compiler_tester/test_program.o -c simple_compiler_tester/test_program.s
mips-linux-gnu-gcc -mfp32 -static -o simple_compiler_tester/test_program simple_compiler_tester/test_program.o
qemu-mips simple_compiler_tester/test_program
echo $?