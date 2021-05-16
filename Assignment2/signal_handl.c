#include "signal_handl.h"
#include "signal.h"
#include "sh_pipes.h"

#include <sys/wait.h>
#include <stdio.h>


int signal_num = 0;
struct MonitorStruct *commun;
extern int numMonitors; 
extern size_t bufferSize;





//SIGNALS FOR PARENT PROCESS
void handle_recreate(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 1;
    }
    //SIGCHLD - Parent
    pid_t child_pid = wait(NULL);
    printf("Child with pid: %d was terminated\n", child_pid);

    for (int i = 0; i < numMonitors; i++){
        
        if (commun[i].pid == child_pid){
            
            create_child(i, bufferSize);
        }

    }
    

    printf("Fork new Monitor Process\n");

    signal_num = 0;

}



void handle_ParentFin(int sig){                 //Parent

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 4;
    }


    

    printf("Printing log_file from Parent\n");

}





//SIGNALS FOR MONITORS

void handle_newfiles(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 2;
    }
    
    //ένα Monitor process λάβει ένα SIGUSR1  - Monitor


    printf("New Files to be Read\n");


}


void handle_MonitorFin(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 3;
    }

    /*Αν ένα Monitor process λάβει ένα signal SIGINT ή          - Monitor
    SIGQUIT τότε τυπώνει σε ένα αρχείο με ονομασία log_file.χχχ 
    (όπου το χχχ είναι το process ID του) το όνομα των χωρών (των subdirectories) 
    που διαχειρίζεται, το συνολικό αριθμό αιτημάτων που δέχθηκε για είσοδο στις 
    χώρες που διαχειρίζεται, και το συνολικό αριθμό αιτημάτων που εγκρίθηκαν 
    και απορρίφθηκαν.*/

    printf("Printing log_file from Monitor\n");
}



void handle_needtoread(int sig){

    if ( signal_num == 0){
        signal_num = 5;
    }

}