# C Minus Compiler

For this project we were tasked with writting a compiler for CMinus which is a subset of the C language. We were allowed to make the compiler in whatever language we wanted and we were allowed to use tools like Flex and Bison. I decided to use C to make the compiler and I wanted to code everything by hand so I didn't use any other tools.

This project was divided up into 3 parts: the lexer, the parser, and the code generation. You can see the 3 folders for each of the parts.

The only thing wrong with the compiler is some bug in the part of the code generation that tracks the scoping of variables and some offset gets screwed up when there are global variables in the program.

I do plan on revisiting this and fixing this bug.

NOTE: the elf executables (geracodigo, sintatico, lexico) in the folders were provided by my professor and were for checking to make sure we were doing that part of the project correctly.

I will clean this repository up eventually too.
