#include "signal_handl.h"
#include "signal.h"
#include "sh_sockets.h"

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int signal_num = 0;
struct MonitorStruct *commun;

extern struct Arguments arg;




//SIGNALS FOR PARENT PROCESS
void handle_recreate(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 1;
    } else {                    //case: command /exit
        return;
    }

    //SIGCHLD - Parent
    pid_t child_pid = wait(NULL);
    printf("Child with pid: %d was terminated\n", child_pid);

    for (int i = 0; i < arg.numMonitors; i++){
        
        if (commun[i].pid == child_pid){

            commun[i].port = -1;
            close(commun[i].sock);
            deleteVirMain(&(commun[i].viruses));
            deleteFileslist(&(commun[i].list_of_countries));

            break;
        }

    }
    

    printf("Fork new Monitor Process\n");


}



void handle_ParentFin(int sig){

    signal_num = 4;

    printf("Printing log_file from Parent\n");

}





//SIGNALS FOR MONITORS

void handle_newfiles(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 2;
    }
    

    printf("New Files to be Read\n");


}


void handle_MonitorFin(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 3;
    }

    printf("Printing log_file from Monitor\n");

}



void handle_needtoread(int sig){    //signal for reading

    if ( signal_num == 0){
        signal_num = 5;
    }

}