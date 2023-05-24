COP3402 - Fall 2021
HW4 PL/0 Compiler
Group Members: Henry Graves and Arthur Upfield

No alterations made to the provided documents.

--- Instructions for running the compiler ---
To compile and run, execute the following commands in a UNIX environment:
gcc driver.c lex.c parser.c vm.c
./a.out <name_of_input_file.txt> <desired compiler directives>

For example:
gcc driver.c lex.c parser.c vm.c
./a.out input.txt -l -s -a -v

--- List of supported compiler directives: ---
The following is from: HW3InstructionsFall2021.doc provided by COP3402 professor and TAs
-l : print the list and table of lexemes/tokens to the screen
-s : print the symbol table
-a : print the generated assembly code to the screen
-v : print virtual machine execution trace to the screen