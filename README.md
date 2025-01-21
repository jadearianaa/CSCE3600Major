# CSCE3600Major
# Major 2
## Group Members
- Reagan Duke
- Amaan Siddiqui
- Joshua Yao 
- Jade Mitchell 
## Organization
### Features | Responsible party
- ### Built-In cd Command & Redirection | Joshua Yao
- ### Built-In exit Command & Pipelining | Jade Mitchell
- ### Built-In path Command & Signal Control | Amaan Siddiqui
- ### Built-In myhistory Command & Alias Support | Reagan Duke
## Known Bugs/Problems
- The following command crashes putty terminal: exit; cd ..
---
## Design Overview
- Each of our feauture compotents are built in tothe same file as the main function, except the path feature.
The shell is run using the following commands:
    - 1. make
    - 2. ./newshell or ./newshell [batchtext]
- Where "./newshell" starts the shell in interactive mode and "./newshell [batchtext]" runs in batch mode.
- Each command is then read into a parser function that utlizes a regex to check command validity before execution.
##  Redirection and cd
### cd
- A built-in cd command that accepts one optional argument, a directory path, and changes the current working directory to that directory. If no argument is passed, the command changes the current working directory to the user’s HOME directory.
- Usage:
    - cd: Change to users HOME directory
    - cd [path]: Change Directory to users specified directory
### Redirection
- Redirects a file to or command to standard input or standard ouput.
- Usage:
    - cmd1 < in.txt: Redirects file to standard input
    - cmd2 > out.txt: Redirects command to standard output
##  Pipe and exit
### Pipe
- A build in pipe() function able to handle up to three commands chained together with the pipeline operator
- Usage:
    - cmd1 | cmd2: Opens pipe between two commands
    - cmd1 | cmd2 | cmd3: Opens pipe chain between three commands
    - This can be tested with any commands, but the best would be the echo, cut -b, and wc commands.
    - Specifically, echo hello | cut -b 1,2,3 should return hel
    - For three comands, echo hello| cut -b 1,2,3| wc returns 1 1 4
### exit
- A built-in exit command that exits from the shell itself with the exit() system call. 
- If the exit command is on the same line as other commands, the other commands execute (and finish) before exiting the shell.
- Usage:
    - exit: Exits the shell
    - exit; cmd1: Runs cmd1 then exits the shell
##  Signal Control and path 

### Signal Control
- A built-in signal handling that applied system signals only to new shell process and it's subgroups.
- Allows signal handling that does not effect original terminal.
- Usage:
    - kill –XXX pid: Sends any signal to the process with process ID pid, where XXX is the human-friendly suffix of the desired signal.
### path
- A built-in path command that allows users to show the current pathname list, append one pathname, or remove one pathname.
- Adds new path to the “real” PATH environment variable for executables in the path to work correctly.
- Assumes there are no duplicate pathnames present and restores PATH environment variable to its original state when the user exits shell.
- Usage:
    - path: Displays the pathnames currently set.
    - path + ./bin: Appends the pathname to the path variable
    - path - ./bin: Removes the pathname to the path variable.
## alias and myhistory 
### alias
- A built-in alias command that allows you to define a shortcut for commands by essentially defining a new command that substitutes a given string for some command, perhaps with various flags/options.
- Usage:
    - alias alias_name='command': Creates alias
    - alias -r alias_name: Removes an alias
    - alias -c: Removes all alias
### myhistory
- A built-in myhistory command that lists the shell history of previous commands run in new shell (not the bash shell).
- Supports a history of 20 most recent commands (i.e., the 21st command will overwrite the 1st command).
- Usage:
    - myhistory: Lists the shell history of previous commands run in your shell
    - myhistory -c: Clears history
    - myhistory -e myhistory_number: executes command whos history number matches myhistory_number
