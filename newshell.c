/*
 * Authors: Joshua Yao (jjy0031), Jade Mitchell (jam1375), Reagan Duke(rmd0171), Amaan Jamil Siddiqui (ajs0576)
 * Date: 11/22/2024
 * Description: Mainfile for newshell
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "PathCommand.h"

//Global variables
regex_t REGEXP;
int exitPresent = 0;
int argCount = 0;

void CD(const char* path){
/*
 * Changes the path of the shell.
 * If a path is given, go to path.
 * Otherwise return go to home directory.
 */
    if(path == NULL ||(strlen(path) == 0)){ // empty path
        // go home
        if(chdir(getenv("HOME"))!= 0) perror("chdir() to HOME failed");
    }
    else {
        // go path
        if(chdir(path)!= 0){
            fprintf(stderr,"chdir() to '%s' failed: ", path);
            perror("");
        }
    }
}
void reDIRECT(char* line, int io ){
    int ifp, ofp, forKid;
    char file[512];
    char cmd[512];
    char *word;
    word = strtok(line, " <>"); // skip direction and whitespace
    strcpy(cmd, word);// expect 1st word to be a cmd
    word = strtok(NULL, " <>"); // skip direction and whitespace
    strcpy(file, word);// expect cmd to be followed by arguement
    char *cmd_args[] = {cmd, file, NULL};
    pid_t pid = fork();
    if(pid == 0 ) // child process
    {
        if(io == 0){// redirect to stdin
            // Open file
            if((ifp = open(file, O_RDONLY)) != -1){
                // dup input to stdin
                dup2(ifp, 0);
                close(ifp);
                execvp(cmd, cmd_args);
                return;
            }
            else perror("Failed to open input file");
        }
        else if (io == 1){// redirect to stdout
            // Open file, if file does not exist, create it
            ofp = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            dup2(ofp, 1);
            close(ofp);
            execvp(cmd, cmd_args);
            return;
        }
        else perror("Failed to select input or output direction");
    }
    else if(pid > 0) wait(&forKid);
    else perror("Could not make child");
    
}

bool shellExit(int exitPresent)
{
    // if an exit command is present returns true
    if(exitPresent == 1){return true;}
    else{return false;}
}

bool removeExit(char* line, char* sub)
{
    char* match;
    bool matchFound = false;
    int length = strlen(sub);
    while((match = strstr(line, sub)))
    {
        matchFound = true;
        *match = '\0';
        strcat(line, match+length);
    }

    return matchFound;
}

void shellPipe(char* line)
{ 
    int status; 
    int cmdcount = 0; // a count of commands for my wait status
    int oldfd = 0; // once there is a command before, this will be the fd for the old command
    char* command =  strtok(line, "|"); // each command gets seperated by | character

    while(command != NULL) // while there a command to perform
    {
        int fd[2]; // created file descriptor 
        if(pipe(fd) == -1)
        {
            perror("Pipe Failed.\n");
            return;
        }
        pid_t pid = fork();

        if(pid < 0)
        {
            perror("Fork Failed.\n");
            return;
        }
        else if(pid == 0)
        {
            //child process
            if(strtok(NULL, "|") != NULL) // while there is a command
            {
                dup2(fd[1], 1); // open write end of new fd
                close(fd[1]); 
            }
            if(oldfd != 0) // means there are 2+ fds
            {
                dup2(oldfd, 0); // opens the reading end of the old fd
                close(oldfd);
            }

            char* cmd[100]; // each argument of the command should be held in this array
            char* args = (strtok(command, " ")); // each argument should be separated by whitespace
            int i = 0; // starts at 0 each time bc first argument is the command to execute
            while(args != NULL)
            {
                cmd[i] = args; // array of arguments
                i++;
                args = strtok(NULL, " "); // emoves to the next argument
            }
            cmd[i] = NULL; // terminates array before calling execute

           if((execvp(cmd[0], cmd)) == -1) //executing command
           {
                perror("Cannot execute command.\n");
                return;
           }
        }
        else 
        {
            //parent process
            if (oldfd != 0) 
            {
                close(oldfd);
            } 
            close(fd[1]); 
            oldfd = fd[0];
        }

        command = strtok(NULL, "|");
        ++cmdcount; // keep track of number of commands

    }

     for (int i = 0; i < cmdcount; i++) // wait for child processes to finish
        {
            wait(&status);
        }
}

#define HISTORY_SIZE 20
char *history[HISTORY_SIZE];
int history_index = 0;
int history_count = 0;

int parse(char *line);

//Add a command to history
void add_history(const char *command)
{
	if (history[history_index] != NULL)
	{
		free(history[history_index]); //free memory for overwritten entry
	}
	history[history_index] = strdup(command); //add command to array
	history_index = (history_index + 1) % HISTORY_SIZE;
	if (history_count < HISTORY_SIZE)
	{
		history_count++;
	}
}

//Manage History
void manage_myhistory(const char *args)
{
	if (args == NULL || (strlen(args) == 0))
	{
		//print history
		for (int i = 0; i < history_count; i++)
		{
			printf("%d: %s\n", i + 1, history[i]);
		}
	}
	else if (strcmp(args, "-c") == 0)
	{
		//clear history
		for (int i = 0; i < HISTORY_SIZE; i++)
		{
			if (history[i] != NULL)
			{
				free(history[i]);
				history[i] = NULL;
			}
		}
		history_index = 0;
		history_count = 0;
		printf("History cleared.\n");
	}
	else if (strncmp(args, "-e", 3) == 0)
	{
		int command_index = atoi(args + 3) -1;
		if (command_index < 0 || command_index >= history_count)
		{
			fprintf(stderr, "Invalid history index.\n");
		}
		else
		{
			int index = (command_index) % HISTORY_SIZE;
			parse(history[index]);	//recursively parse the selected command
		}
	}
	else
	{
		fprintf(stderr, "Invalid option for myhistory.\n");
	}
}

#define MAX_ALIASES 50
typedef struct 
{
    char alias_name[50];
    char command[512];
} Alias;

Alias aliases[MAX_ALIASES];
int alias_count = 0;

// Add or update an alias
void add_alias(const char *name, const char *command) {
    for (int i = 0; i < alias_count; i++) 
	{
        if (strcmp(aliases[i].alias_name, name) == 0) 
		{
            strncpy(aliases[i].command, command, 512);
            return;
        }
    }
    if (alias_count < MAX_ALIASES) 
	{
        strncpy(aliases[alias_count].alias_name, name, 50);
        strncpy(aliases[alias_count].command, command, 512);
        alias_count++;
    } 
	else 
	{
        fprintf(stderr, "Alias limit reached.\n");
    }
}

// Remove a single alias or all aliases
void remove_alias(const char *name) 
{
    if (strcmp(name, "-c") == 0) 
	{
        alias_count = 0; // Clear all aliases
    } 
	else 
	{
		int found = 0;
        for (int i = 0; i < alias_count; i++) 
		{
            if (strcmp(aliases[i].alias_name, name) == 0) 
			{
                for (int j = i; j < alias_count - 1; j++) 
				{
					found = 1;
                    aliases[j] = aliases[j + 1]; //remove a single alias
                }
                alias_count--;
                break;
            }
        }
		if (!found)
		{
			fprintf(stderr, "Alias '%s' not found.\n", name);
		}
    }
}

// Replace aliases in commands
char* replace_alias(const char *command) 
{
    for (int i = 0; i < alias_count; i++) 
	{
        if (strncmp(command, aliases[i].alias_name, strlen(aliases[i].alias_name)) == 0) 
		{
            return aliases[i].command;
        }
    }
    return NULL;
}

// Handle alias commands
void handle_alias(char *line) 
{
    char *name = strtok(line, "=");
    char *command = strtok(NULL, "'");
    if (name != NULL && command != NULL) 
	{
        add_alias(name, command); //add alias
		printf("Alias '%s' set to '%s'\n", name, command);
    } 
	else if (strcmp(line, "-c") == 0) 
	{
        remove_alias("-c"); //clear all alias
		printf("All aliases cleared.\n");

    } 
	else if (strncmp(line, "-r ", 3) == 0) 
	{
        remove_alias(line + 3); //remove 1 alias
    } 
	else 
	{
        for (int i = 0; i < alias_count; i++) 
		{
            printf("%s='%s'\n", aliases[i].alias_name, aliases[i].command);  //prints all alias
        }
    }
}

int parse(char* line){
/*
 * Parses a line in search of of commands.
 * If a command exists, execute command.
 * Otherwise return no command error.
 */
    // trim new line characters
    line[strcspn(line, "\n")] = 0;

    char *alias_replacement = replace_alias(line);
    if(alias_replacement != NULL)
	{
	    strcpy(line, alias_replacement);
	} 
	
	if (strncmp(line, "alias ", 6) == 0) 
	{
		char *temp_line = strtok(line + 6, " ");
		handle_alias(temp_line);
		return 1;
	}
	else if (strcmp(line, "alias") == 0)
	{
		handle_alias(line);
		return 1;
	}

    else if (strncmp(line,"path",4) == 0){                 //check to see if command starts witht the "path" keyword signaling a pathcommand
        Create_PList();                 //initialize the Path_lists array          
        CHECK_Path_Command(line);       //pass command as argument and either display current path, append a path, or remove a path form Path_lists variable
        return 1;
    }

    int cmds = regcomp(&REGEXP, "myhistory|exit|cd|\\||<|>", REG_EXTENDED);
    if(cmds != 0) {
        printf("ERROR: REGEXP complilation error.\n");
        return 1;
    }
    cmds = regexec(&REGEXP, line, 0, NULL, 0);
    if (cmds == REG_NOMATCH){
        printf("'%s' does not contain any inbuilt commands. Please try again.\n", line);
        return 1;
    }

    printf("Executing: %s\n", line);

    if(removeExit(line, "exit"))
    {
        ++exitPresent; // removes exit from the line to execute other commands easily
    }
    if(line == NULL || (strlen(line) == 0))
    {
        //Path_Restore();
        exit(0); // if line is empty, means there was only an exit on the line
    }

    if(strchr(line, '|') != NULL)
    {   
        shellPipe(line);
    }
    else if(strchr(line, '<') != NULL){
        reDIRECT(line, 0);
    }
    else if(strchr(line, '>') != NULL){
        reDIRECT(line, 1);
    }
    else{
        char *word;
        word = strtok(line, " ;");
        do{
            argCount++;
            if(strcmp(word, "cd") == 0){
                word = strtok(NULL, " ;");
                CD(word);
            }
            else if (strncmp(line, "myhistory", 10) ==0)
	        {
		    char *args = strtok(line + 10, " ");
		    manage_myhistory(args);
	        }  
            word = strtok(NULL, " ;");
        }while(word != NULL);
    }
    if(shellExit(exitPresent)) {
        //Path_Restore();                     //Restore original path before exiting
        exit(0);
    }

    add_history(line);

    return 1;
}


int main(int argc, char* argv[]){
    char line[512]; // batch line
    char *iline = NULL; // interactive line
    size_t line_size = 512;
    char dir[512];
    int s_size = 0;
    //Force valid arguments
    if(argc == 1){
        printf("Interactive Mode\n");
        while(1){
            printf("newshell%s: ", getcwd(dir,512));
            s_size = getline(&iline, &line_size, stdin);
            if(s_size > 0){
                parse(iline) ;
            }
        }
    }
    else if(argc == 2){
        printf("Batch Mode\n");
        // set file descriptor
        FILE* fd1;
        // open file
        fd1 = fopen(argv[1], "r");
        // check for failure
        if(fd1 == NULL){
            printf("ERROR: failed to open file, exiting program\n");
            exit(-1);
        }
        // read, echo, and execute commands
        while(fgets(line, sizeof(line), fd1) != NULL){
            printf("%s\n", line);
            parse(line) ;
        }
    }
    else printf("Usage: ./newshell [batchFile] | batchfile is optional\n");

    //Free resources
    regfree(&REGEXP);
    return 0;
}
