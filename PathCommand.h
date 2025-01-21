#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *Path_lists[1000];                          //array to deal with pathname list 

char *origin;                    //initial PATH environment variable

void Path(){                    //function that implements the built in path command given a command line to show the current path to the user

    for(int i = 0;Path_lists[i] != NULL; i++){              //parse throught the path list until NULL terminator is encountered
        printf("%s", Path_lists[i]);            //print out path contained within Path_lists[i]

        if(Path_lists[i+1] != NULL){printf(":");}            //ensure that next path is not NULL terminator, and if it is not then insert a comma to create path seperation
    }
    printf("\n");   //print newline for formatting
}

void Path_append(char *path_add){   //function that implements the built in path to append a pathname
    char buffer[2000] = {0};                          //buffer to create new path variable (with appended path) to replace old path variable

    // Skip leading './' if present
    if (strncmp(path_add, "./", 2) == 0) {
        path_add += 1; // Move the pointer to skip the '.'
    }

    int i = 0;                  //variable used to index Path_lists
    while(Path_lists[i] != NULL){i++;}                  //determine where the last path is

    if (i < 999){           //insure index is not out of bounds
        Path_lists[i] = path_add;               //append to the path
        Path_lists[i+1] = NULL;                 //null terminate the list after the inserted path is appended

        for(int i = 0; Path_lists[i] != NULL; i++){
            strcat(buffer, Path_lists[i]);                  //concatenate all paths currently within list and store within the buffer    
            
             if(Path_lists[i+1] != NULL){strcat(buffer,":");}            //ensure that next path is not NULL terminator, and if it is not then insert a comma to concatenate a comma
        }

        setenv("PATH",buffer,1);                //update the path variable and replace with concatenated string in buffer
        return;
    }            
    else{
        printf("Error: Maximum number of paths reached. Cannot append new path.\n");
        return;
    }
}

void Path_remove(char *path_del){               //function that implements the built in path to remove a pathname
    char buffer[2000] = {0};                          //buffer to create new path variable (with appended path) to replace old path variable
   
    // Skip leading './' if present
    if (strncmp(path_del, "./", 2) == 0) {
        path_del += 1; // Move the pointer to skip the '.'
    }

    int i = 0;              //variable used to indes Path_lists
    while(Path_lists[i] != NULL){           //parse Path_lists
        if(strcmp(Path_lists[i],path_del) ==  0){                   //find which value within the Path variable matches the path that needs to be removed
            free(Path_lists[i]);                //remove the values

            for(int j = i; Path_lists[j] != NULL; j++){                 //parse at the position where deletion happened
                Path_lists[j] = Path_lists[j+1];            //shift values to ensure the buffer can create a continuous string
            }

             for(int i = 0; Path_lists[i] != NULL; i++){
            strcat(buffer, Path_lists[i]);                  //concatenate all paths currently within list and store within the buffer    
            
             if(Path_lists[i+1] != NULL){strcat(buffer,":");}            //ensure that next path is not NULL terminator, and if it is not then insert a comma to concatenate a comma
            }

            setenv("PATH",buffer,1);                //update the path variable and replace with concatenated string in buffer
            return;
        }
        i++;            //iterate through if the value is not found
    }
    
}

void CHECK_Path_Command(char *Line_Command){                    //function checks to see if a path command was passed and directs to append, remove, or display passed path to function

    char *Path_Command = Line_Command + 4;                  //skip the "path" keyword since already identified, and then 
    while (*Path_Command == ' ') Path_Command++;                //skip any leading spaces to get the next character

            if (*Path_Command == '\0'){          //if only "path" was the command then display the current path to the user
                Path();
                return;
            }
            else if (*Path_Command == '+'){         //if "path +" was detected, then append the path with path provided
                Path_append(Path_Command + 2);          //pass to Path_append with what needs to be added to the path variable
                return;
            }
            else if (*Path_Command == '-'){     //if "path -" was detected, then remove the specified path
                Path_remove(Path_Command + 2);              //pass to Path_remove with what needs to be removed from the path variable
                return;
            }
            else {
                printf("Error: Invalid path command. Please use try again with valid syntax.\n");       //if not a path command with valid syntax, return back to main
            }
    return;
}

void Path_Restore() {                       /// Function to restore the original PATH environment variable and clean up memory (called upon exiting the program)
    setenv("PATH", origin, 1);               // Reset PATH to its original value
    
    for (int i = 0; Path_lists[i] != NULL; i++) {                       // Free all dynamically allocated paths in the list (empty so no memory leaks)
        free(Path_lists[i]);
    }
}

void Create_PList(){                    //Function to initialize and populate the Path_Lists array upon starting the shell
    origin = getenv("PATH");        //Save initial PATH variable so it can be recovered upon exiting the program

    char *copy = strdup(origin);         //duplicate original path variable so that it can be tokenized

    char *token = strtok(copy, ":");   //tokenize the copied path variable by inserting ":" between the tokens

    int i=0;            //variable used to index Path_Lists
    while ((token != NULL) && (i<1000)) {                   //insert into Path_lists so long as there are paths to copy
     Path_lists[i++] = strdup(token);           //store token into the array
     token = strtok(NULL, ":");            //move to the next token using ":" as the delimeter

}
    Path_lists[i] = NULL;                  //Mark end of list with Null to ensure easy parsing
    free(copy);                 //free the duplicated path variable
}