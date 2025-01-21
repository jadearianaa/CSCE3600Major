#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include <sys/types.h>
#include<sys/wait.h>


pid_t shell_pgid;           //process group id of the shell
pid_t fg_pid = -1;          //process group ID of foreground process
int SHELL_DESCRIPTOR;       //shell file descriptor

void Restore_Control(){         //function to restore foreground process control
    tcsetpgrp(SHELL_DESCRIPTOR,shell_pgid);    //set the shell's process group back to the parent's process group (restor foreground process)
}

void handler(int sig){          //function to manage signals
    if (fg_pid > 0) {               //ensure that a foreground process is detected
        // If there's a foreground process group, send the signal to it
        kill(-fg_pid, sig);  // Send the signal to the entire process group
    }
}

void Parent_launch(pid_t child){               //function to manage signals for a parent process
    int status;
    setpgid(child,child);

    if(fg_pid == 0)         //check to see if the proces is a foreground process
    {
         fg_pid = child;
         tcsetpgrp(SHELL_DESCRIPTOR,child);            //set the process to have terminal control

         waitpid(child,&status,WUNTRACED);            //wait for foreground process to finish or stop

         fg_pid = -1;                       //Get rid of foreground process by resetting fg_pid

         Restore_Control();                 //return terminal control to the shell
    }
    
}

void Child_launch(){        //function to manage signals for a child process
    setpgid(0,0);               //set child into its own process group

    if (fg_pid == 0){       //check to see if the process is a foreground process
        tcsetpgrp(SHELL_DESCRIPTOR,getpid());            //set the process to have terminal control
    }

    signal(SIGINT,SIG_DFL);     //restore default Ctrl-C handler
    signal(SIGTSTP,SIG_DFL);    //restore default Ctrl-Z handler

    return;
}

void Start_shell(){     //function to assign initial signal handling behavior upon startup
        SHELL_DESCRIPTOR = STDIN_FILENO;    //assign the shell's file descriptor to that of the standard input
        shell_pgid = getpid();              //assign the the shell it's own process group ID

        if(setpgid(shell_pgid,shell_pgid) < 0){                         //check to see fi the process group was successfully created
            perror("Failed to set process group to shell.\n");
            exit(1);            //if not created properly, exit the shell
        }  
        
        tcsetpgrp(SHELL_DESCRIPTOR,shell_pgid);             //set the current interactive shell to be the forground process group
        
        signal(SIGINT,handler);     //handle Ctrl-C and
        signal(SIGTSTP,handler);        //handle Ctrl-Z
}

