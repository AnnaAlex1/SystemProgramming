#include "console.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include<sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>

#include "signal_handl.h"
#include "records.h"
#include "sh_pipes.h"
#include "citizens.h"
#include "countries.h"


extern int size_in_bytes;   //-s
size_t bufferSize = 10000;             //-b
int numMonitors;    //-m 

void distribute_subdirs(char * input_dir, struct MonitorStruct *commun, CountryMainHash countries);


int main( int argc, char *argv[]){


    //CHECK ARGUMENTS
    if ( argc != 9 ){
        printf("Wrong number of arguments!\n");
        return -1;
    }


    
    char *input_dir;    //-i

    

    //check correctness of arguments
    if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-i") == 0)){      //-m, -b, -s, -i

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);
        

    } else if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){  //-m, -b, -i, -s
        
        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-m, -s, -b, -i

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ((strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-b") == 0)){    //-m, -s, -i, -b

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-m, -i, -b, -s

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);


    } else if ((strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-b") == 0)){    //-m, -i, -s, -b

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);


    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-b, -m, -s, -i

        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);


    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-b, -m, -i, -s

        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-b, -i, -s, -m

        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-b, -i, -m, -s

        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-b, -s, -i, -m

        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-b, -s, -m, -i
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-s, -b, -m, -i
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-s, -b, -i, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-s, -m, -b, -i
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-s, -m, -i, -b
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-s, -i, -m, -b
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-s, -i, -b, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-i, -s, -m, -b
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-i, -s, -b, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-i, -b, -m, -s
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-i, -b, -s, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-i, -m, -s, -b
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-i, -m, -b, -s
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else {

        printf("Wrong Input!\n");
        return -1;
    }


    printf("Size of Bloom: %d\n", size_in_bytes);
    printf("Num of Monitors: %d\n", numMonitors);
    printf("BufferSize: %ld\n", bufferSize);
    printf("Directory for Input: %s\n\n", input_dir);

    
    //Hashtable of Countries
    CountryMainHash countries;
    countries = hashtable_createCounMain();


    //Δημιουργία named pipes
    char w_pname[] = "pipes/wfifo";
    char r_pname[] = "pipes/rfifo";
    char *extw_pname;
    char *extr_pname;
    char number[5];

    //int fd[2][numMonitors];
    //int pids[numMonitors];
    struct MonitorStruct commun[numMonitors];
    for (int i = 0; i < numMonitors; i++){
        commun[i].viruses = NULL;
    }
    


    for (int i=0; i<numMonitors; i++){



        //PIPE FOR WRITING
       
        //BUILD PIPE NAME
        sprintf(number, "%d", i);        //convert counter to string
        extw_pname = malloc( sizeof(char) * ( strlen(w_pname) + strlen(number) + 1) );
        strcpy(extw_pname, w_pname);
        strcat(extw_pname, number);       //add counter at the end of string
        printf("Name: %s\n", extw_pname);


        //CREATE NAMED PIPE FOR WRITING
        if ( mkfifo( extw_pname, 0666) == -1){
                        
            if ( errno != EEXIST ){
                perror("ERROR: creating Named Pipe");
                exit(6);
            }
        }

        commun[i].fifoname_w = malloc( sizeof(char) * (strlen(extw_pname)+1));
        strcpy(commun[i].fifoname_w, extw_pname);



        //PIPE FOR READING
 
        //BUILD PIPE NAME
        extr_pname = malloc( sizeof(char) * ( strlen(r_pname) + strlen(number) + 1) );
        strcpy(extr_pname, r_pname);
        strcat(extr_pname, number);       //add counter at the end of string
        printf("Name: %s\n", extr_pname);



        //CREATE NAMED PIPE FOR WRITING
        if ( mkfifo( extr_pname, 0666) == -1){
                        
            if ( errno != EEXIST ){
                perror("ERROR: creating Named Pipe");
                exit(6);
            }
        }

        commun[i].fifoname_r = malloc( sizeof(char) * (strlen(extr_pname)+1));
        strcpy(commun[i].fifoname_r, extr_pname);




        //FORK CHILD PROCESS
        commun[i].pid = fork();

        if ( commun[i].pid == -1 ){
            perror("ERROR: in fork");
            exit(1);
        }

        if ( commun[i].pid == 0 ){
            //this is a child process

            char *args[] = {"./monitor", extw_pname, extr_pname, NULL};

            //execute Monitor Process
            if (execv(args[0], args)){
                perror("ERROR: in exec");
                exit(1);
            }
        }


        commun[i].fd_w = open(extw_pname, O_WRONLY);
        if ( commun[i].fd_w < 0 ){
            perror("ERROR: in opening Named Pipe (from Monitor)");
            exit(1);
        }

        commun[i].fd_r = open(extr_pname, O_RDONLY);
        if ( commun[i].fd_r < 0 ){
            perror("ERROR: in opening Named Pipe (from Monitor)");
            exit(1);
        }



        char mes_to_sent[sizeof(long)];
        sprintf(mes_to_sent, "%ld", bufferSize);

        sent_message_wrong(commun[i].fd_w, mes_to_sent, sizeof(long));

        sprintf(mes_to_sent, "%d", size_in_bytes);
        sent_message_wrong(commun[i].fd_w, mes_to_sent, sizeof(int));


        free(extw_pname);
        free(extr_pname);

    }

    printf("\nDistribution of folders:\n");
    distribute_subdirs(input_dir, commun, countries);

    

    //ΑΝΑΜΟΝΗ ΓΙΑ BLOOMFILTERS
    if (get_bloomfilters(commun, bufferSize, numMonitors) == -1){
        perror("ERROR in getting bloomfilters");
        exit(1);
    }


    //ΛΗΨΗ ΜΗΝΥΜΑΤΟΣ ΕΤΟΙΜΟΤΗΤΑΣ
    char *ready = NULL;
    for (int i = 0; i < numMonitors; i++){

        /*if ( ready != NULL ){
            
            do{
                ready = get_message_wrong(commun[i].fd_r, bufferSize);

            } while ( strcmp(ready, "READY") != 0 );


        }*/

        ready = get_message_wrong(commun[i].fd_r, bufferSize);
        if ( strcmp(ready, "READY") != 0 ){
            printf("Something Went Wrong!\n");
        } else {
            printf("Monitor %d is ready!\n", commun[i].pid);
        }
        
    }
    

    static struct sigaction sa1;        //SIGINT   Ctrl+C
    static struct sigaction sa2;        //SIGQUIT   Ctrl + \ ///
    static struct sigaction sa3;        //SIGCHLD   kill -s CHLD pid

    sa1.sa_handler = handle_ParentFin;
    sigfillset (&(sa1.sa_mask));
    //sigaction(SIGINT , &sa1 , NULL);

    sa2.sa_handler = handle_ParentFin;
    sigfillset (&(sa2.sa_mask));
    sigaction(SIGQUIT , &sa2 , NULL);

    sa3.sa_handler = handle_recreate;
    sigfillset (&(sa3.sa_mask));
    sigaction(SIGCHLD , &sa3 , NULL);


    ////////////////////////////////////
    //CONSOLE
    console(commun, countries);


    //SIGPROCMASK ΚΑΤΑ ΤΗΝ ΑΠΟΣΤΟΛΗ ΔΕΔΟΜΕΝΩΝ -> ΜΠΛΟΚΑΡΙΣΜΑ ΜΗΝΥΜΑΤΩΝ

   int status;
   

   for (int i = 0; i < numMonitors; i++){
        waitpid(commun[i].pid, &status, 0);
        printf("PID: %d, status: %d\n",commun[i].pid, status);
        printf("WTERMSIG(status)= %d\n", WTERMSIG(status));
        printf("WIFEXITED(status)= %d\n", WIFEXITED(status));
   }
   


    printf("\nFinished with Pipes\n");
    
    for (int i = 0; i < numMonitors; i++) {
        close(commun[i].fd_w);
        close(commun[i].fd_r);
        unlink(commun[i].fifoname_w);
        unlink(commun[i].fifoname_r);

        free(commun[i].fifoname_w);
        free(commun[i].fifoname_r);

        //deleteVirMain(&(commun[i].viruses));
    }
    

    
    

    

    hashtable_destroyCounMain(countries);
    free(countries);

    return 0;
}













void distribute_subdirs(char * input_dir, struct MonitorStruct *commun, CountryMainHash countries){

    //READ FILES
    DIR *maindr;
    struct dirent *dsub_dir;


    //open directory input_dir
    if ( (maindr = opendir(input_dir)) == NULL ) { perror("Directory cannot open!\n"); exit(1); }

    int i=0;

    //for every subdirectory
    while ( (dsub_dir = readdir(maindr)) != NULL ){

        //printf("%s\n", dsub_dir->d_name);

        if ( (strcmp(dsub_dir->d_name,".") != 0) && (strcmp(dsub_dir->d_name,"..") != 0) ){

            //put country in hashtable
            hashtable_addCounMain(countries, dsub_dir->d_name, commun[i].pid);

            printf("    Opening Directory: %s\n", dsub_dir->d_name);

            char *path;
            path = malloc(sizeof(char) * ( strlen(dsub_dir->d_name) + strlen(input_dir) + 2 ) );
            //path=malloc(bufferSize);
            strcpy(path, input_dir);
            strcat(path, "/");
            strcat(path, dsub_dir->d_name);

            //printf("File descriptor: fd[%d]=%d, subdirectory: %s\n", i, fd[i], path);
            sent_message_wrong(commun[i].fd_w, path, bufferSize);

            i = (i + 1) % numMonitors;

        }

    }

    printf("\nFinished Distribution\n");
    char *mes = malloc(bufferSize);
    strcpy(mes, "DONE");
    for (int i = 0; i < numMonitors; i++){
        sent_message_wrong(commun[i].fd_w, mes, bufferSize);
    }
    free(mes);
    printf("Finished Sending DONE signal\n");

}