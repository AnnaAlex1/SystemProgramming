#include "signal_handl.h"
#include "signal.h"


#include <stdio.h>


int signal_num = 0;


void handle_recreate(int sig){

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 1;
    }
    //SIGCHLD - Parent

    printf("Fork new Monitor Process\n");

}


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


void handle_ParentFin(int sig){                 //Parent

    if ( signal_num == 0){      //if it's not used by another signal
        signal_num = 4;
    }

    printf("Printing log_file from Parent\n");

}



//PARENT

    // ΣΗΜΑ SIGCHLD
    /* ΑΝ ΕΝΑ ΜΟΝΙΤΟΡ ΣΤΑΜΑΤΗΣΕΙ ΞΑΦΙΝΚΑ 
    θα πρέπει το parent process να κάνει fork νέο Monitor 
    process που θα το αντικαταστήσει.
    */
    

    /*ΣΗΜΑ SIGINT ή SIGQUIT

    πρώτα να τελειώσει την επεξεργασία της τρέχουσας εντολής από το χρήστη 
    Και αφού έχει απαντήσει στο χρήστη, θα στέλνει ένα SIGKILL σήμα στους Monitors, 
    θα τους περιμένει να τερματίσουν, και στο τέλος 
    θα τυπώνει σε ένα αρχείο με ονομασία log_file.χχχ όπου το χχχ είναι το process ID του,
    το όνομα όλων των χωρών (των subdirectories) που συμμετείχαν στην εφαρμογή με 
    δεδομένα, το συνολικό αριθμό αιτημάτων που δέχθηκε για είσοδο στις χώρες, 
    και το συνολικό αριθμό αιτημάτων που εγκρίθηκαν και απορρίφθηκαν.
*/



//MONITOR

    //Λήψη SIGINT/SIGQUIT
        //Άνοιγμα αρχείου logfilexxx
        //εκτύπωση χωρών
        //εκτύπωση total_request
        //εκτύπωση accepted
        //εκτύπωση rejected

