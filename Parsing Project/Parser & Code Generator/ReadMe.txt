
Project 3

// Names: Karim Elsheikh
          Jose Ocasio

Instructions to run the Compiler Driver:

1. Login to Eustis Server using Cisco AnyConnect and Putty as the Client.

2. Using WinSCP, move the Compile folder to the server directory, make sure you have the input file named input.txt in the correct format (same directory)

3. In the command line, type cd Compile to change to the directory you just created in the server

4. Type gcc compile.c -o compile in the command line to compile the project on the linux server.

5. Type ./compile in the command line to run the Compiler Driver. Several output files will be generated if you don't add any compile directives.

You can add optional directives as well. The optional directives are:

- a outputs the m code to the screen
- l outputs the lexeme list to the screen
- v outputs the stack trace to the screen