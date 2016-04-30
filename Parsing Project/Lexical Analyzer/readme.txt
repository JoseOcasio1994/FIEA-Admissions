// Names: Karim Elsheikh
	  Jose Ocasio

Instructions to run Lexical Analyzer:

1. Login to Eustis Server using Cisco AnyConnect and Putty as the Client.

2. Using WinSCP, move the scanner folder to the server directory, make sure you have the input file named input.txt in the correct format (same directory)

3. In the command line, type cd scanner to change to the directory you just created in the server

4. Type gcc scanner.c -o scanner in the command line to compile the project on the linux server (there are no input parameters)

5. Type ./scanner in the command lineto run the Lexical Analyzer. Output files called cleaninput.txt, lexemelist.txt, and lexemetable.txt should be created inside the LexicalAnalyzer server-side directory (same directory as in previous steps.)

