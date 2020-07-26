# myshell
A Simple Shell

This program replaces the command shell in a Unix environment.

After started, it prints a prompt “#” and reads a command line terminated by newline.

# Notes:

the only supported delimiter is the whitespace character
no "special" characters are handled (i.e. quotation marks, backslashes, and tab characters)

# Built-in Commands:

\#  changedir directory

Short for change directory. It is used to change the current directory (which must be an internal variable) to the specified one. If the specified directory does not exist, it will keep the old directory and write an error message. 

\# whereami

Prints the current directory.

\# lastcommands [-c]

Without the parameter, prints out the recently typed commands (with their arguments). If “-c” is passed, it clears the list of recently typed commands 

\# quit

terminates the mysh shell

\# run 'program' [parameters]

The argument “program” is the program to execute. If the argument starts with a “/” (such as /usr/bin/xterm, the shell will interpret it as a full path. Otherwise, the program will be interpreted as a relative path starting from the current directory. 

The program will be executed with the optional “parameters”. It uses fork() + exec() to start the program with the corresponding parameters, and waits until the program terminates (use the waitpid() call). 
For instance

     run /usr/bin/xterm –bg green

would bring up a terminal with a green background. The prompt would not return until the terminal is closed.

(Displays an error message if the specified program cannot be found or cannot be executed.)

\# background 'program' [parameters]

This is similar to the run command, but it immediately prints the PID of the program it started, and returns the prompt. 

\# exterminate PID

Immediately terminates the program with the specific PID (presumably started from this command line interpreter). 

(Uses the kill() function call to send a SIGKILL signal to the program. Displays success or failure.)

\# repeat n 'command' …

Interprets n as the number of times the command must be run, command as the full path to the program to execute, and the others as parameters.  The command starts the specified number of program instances, print the PIDs of the created processes and then return to the prompt, without waiting for the processes to terminate. For instance:

     repeat 4 /usr/bin/xterm 
     
would bring up 4 terminals and print out something like:
PIDs: 31012, 31013, 31014, 31015.

\# exterminateall

Immediately terminates all the programs previously started by the shell which had not been previously terminated by it, or by exterminate. It should output something like:

Murdering 3 processes: 16000 31012 31013
