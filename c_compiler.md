Deliverable 3: A compiler for the C language
============================================

Your program should read C source code from a file, and write MIPS assembly to another file.

Program build and execution
---------------------------

Your program should be built using the command:

    make bin/c_compiler

The compilation function is invoked using the flag `-S`, with the source file and output file specified on the command line:

    bin/c_compiler -S [source-file.c] -o [dest-file.s]
    
You can assume that the command-line arguments will always be in this order, and that there will be no spaces in source or destination paths.

Input 
-----

The input format is the same as for the translator (pre-processed C90).

Here is a list of basic features that you might like to implement first.

* a file containing just a single function with no arguments
* variables of `int` type
* local variables
* arithmetic and logical expressions
* if-then-else statements
* while loops

Here is a list of intermediate features that you might like to implement once the basic features are working.

* files containing multiple functions that call each other
* functions that take up to four parameters
* for loops
* arrays declared globally (i.e. outside of any function in your file)
* arrays declared locally (i.e. inside a function)
* reading and writing elements of an array
* recursive function calls
* the `enum` keyword
* `switch` statements
* the `break` and `continue` keywords

Here is a list of more advanced features like you might like to implement once the basic and intermediate features are working.

* variables of `double`, `float`, `char`, `unsigned`, structs, and pointer types
* calling externally-defined functions (i.e. the file being compiled declares a function, but its definition is provided in a different file that is linked in later on)
* functions that take more than four parameters
* mutually recursive function calls
* locally scoped variable declarations (e.g. a variable that is declared inside the body of a while loop, such as `while(...) { int x = ...; ... }`.
* the `typedef` keyword
* the `sizeof(...)` function (which takes either a type or a variable)
* taking the address of a variable using the `&` operator
* dereferencing a pointer-variable using the `*` operator
* pointer arithmetic
* character literals, including escape sequences like `\n`
* strings (as NULL-terminated character arrays)
* declaration and use of structs

Your compiler will be assessed using test inputs that exercise the above features. No feature not listed above will be tested. 
Here is a (partial) list of features that will not be tested.

* multithreading
* the `goto` keyword
* macros and other preprocessing directives
* the comma operator (for sequencing within expressions)
* the [old K&R style of declaring functions](https://stackoverflow.com/a/18820829)
* union types
* variable-length arrays
* the `const` keyword

All test inputs will be valid; that is, you can assume the absence of programmer errors like syntax faults, type mismatches, and array out-of-bounds errors. The entire compilation and testing process (including compilation, assembly, linking, and MIPS simulation) is expected to complete within ten seconds per program (which should be plenty of time!), and is expected not to use an inordinate amount of memory or disk space. There is no requirement for the generated assembly to be optimised in any way -- the only requirement is that it produces the correct answer.


Output Format
-------------

The output format should be MIPS1 assembly code.

It should be possible to assemble and link this code against a C run-time, and have it execute correctly on a MIPS processor as emulated by `qemu-mips`.

For instance, suppose I have a file called `test_program.c` that contains:

    int f() { return 10; }
    
and another file called `test_program_driver.c` that contains:

    int f();
    int main() { return !( 10 == f() ); }
    
I run the compiler on the test program, like so:

    bin/c_compiler -S test_program.c -o test_program.s
    
I then use GCC to assemble the generated assembly program (`test_program.s`), like so:

    mips-linux-gnu-gcc -mfp32 -o test_program.o -c test_program.s
    
I then use GCC to link the generated object file (`test_program.o`) with the driver program (`test_program_driver.c`), to produce an executable (`test_program`), like so:

    mips-linux-gnu-gcc -mfp32 -static -o test_program test_program.o test_program_driver.c

I then use QEMU to simulate the executable on MIPS, like so:

    qemu-mips test_program
    
This command should produce the exit code `0`.
