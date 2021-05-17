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
size_t bufferSize;             //-b
extern int numMonitors;    //-m 
extern struct MonitorStruct *commun;

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

    commun = malloc( sizeof(struct MonitorStruct)*numMonitors);
    for (int i = 0; i < numMonitors; i++){
        commun[i].viruses = NULL;
    }
    
    for (int i=0; i<numMonitors; i++){

        create_child(i, bufferSize);

    }


    
    printf("\nDistribution of folders:\n");
    distribute_subdirs(input_dir, commun, countries);

    
    
    //ΑΝΑΜΟΝΗ ΓΙΑ BLOOMFILTERS
    for (int i = 0; i < numMonitors; i++){      //for each monitor

        if (get_bloomfilters(commun, bufferSize, numMonitors, i, 0) == -1){
            perror("ERROR in getting bloomfilters");
            exit(1);
        }

    }

    
    //ΛΗΨΗ ΜΗΝΥΜΑΤΟΣ ΕΤΟΙΜΟΤΗΤΑΣ
    char *ready = NULL;
    for (int i = 0; i < numMonitors; i++){

        ready = get_message(commun[i].fd_r, bufferSize);
        if ( strcmp(ready, "READY") != 0 ){
            printf("Something Went Wrong!\n");
        } else {
            printf("Monitor %d is ready!\n", commun[i].pid);
        }
        
        free(ready);
    }
    

    static struct sigaction sa1;        
    static struct sigaction sa2;        
    static struct sigaction sa3;        //SIGCHLD   kill -s CHLD pid

    sa1.sa_handler = handle_ParentFin;
    sigfillset (&(sa1.sa_mask));
    sigaction(SIGINT , &sa1 , NULL);

    sa2.sa_handler = handle_ParentFin;
    sigfillset (&(sa2.sa_mask));
    sigaction(SIGQUIT , &sa2 , NULL);

    sa3.sa_handler = handle_recreate;
    sigfillset (&(sa3.sa_mask));
    sigaction(SIGCHLD , &sa3 , NULL);


    
    ////////////////////////////////////
    //CONSOLE
    console(commun, countries, bufferSize);


    hashtable_destroyCounMain(countries);
    free(countries);
    free(input_dir);

    return 0;
}













void distribute_subdirs(char * input_dir, struct MonitorStruct *commun, CountryMainHash countries){

    //READ FILES
    DIR *maindr;
    struct dirent *dsub_dir;
    char *path;


    //open directory input_dir
    if ( (maindr = opendir(input_dir)) == NULL ) { perror("Directory cannot open!\n"); exit(1); }

    int i=0;

    //for every subdirectory
    while ( (dsub_dir = readdir(maindr)) != NULL ){

        //printf("%s\n", dsub_dir->d_name);

        if ( (strcmp(dsub_dir->d_name,".") != 0) && (strcmp(dsub_dir->d_name,"..") != 0) ){

            //put country in hashtable
            hashtable_addCounMain(countries, dsub_dir->d_name, commun[i].pid);

            //printf("    Opening Directory: %s\n", dsub_dir->d_name);

            path = malloc(sizeof(char) * ( strlen(dsub_dir->d_name) + strlen(input_dir) + 2 ) );
            strcpy(path, input_dir);
            strcat(path, "/");
            strcat(path, dsub_dir->d_name);

            //printf("File descriptor: fd[%d]=%d, subdirectory: %s\n", i, fd[i], path);
            send_message(commun[i].fd_w, path, strlen(path)+1, bufferSize);

            i = (i + 1) % numMonitors;

            free(path);
        }

    }

    printf("\nFinished Distribution\n");
    char *mes = malloc(bufferSize);
    strcpy(mes, "DONE");
    for (int i = 0; i < numMonitors; i++){
        send_message(commun[i].fd_w, mes, strlen("DONE")+1, bufferSize);
    }
    free(mes);
    //printf("Finished Sending DONE signal\n");

    closedir(maindr);
}



