#include "console.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>


#include "signal_handl.h"
#include "records.h"
#include "sh_sockets.h"
#include "citizens.h"
#include "countries.h"


extern struct Arguments arg;
extern struct MonitorStruct *commun;
extern int sizeofbloom;

void distribute_subdirs(char * input_dir, struct MonitorStruct *commun, CountryMainHash countries);


int main( int argc, char *argv[]){


    //CHECK ARGUMENTS
    if ( argc != 13 ){
        printf("Wrong number of arguments!\n");
        return -1;
    }



    //check correctness of arguments
    if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-c") == 0) &&  (strcmp(argv[7], "-s") == 0)  
        &&  (strcmp(argv[9], "-i") == 0) && (strcmp(argv[11], "-t") == 0) ){      
            //-m, -b, -c, -s, -i, -t

        arg.numMonitors = atoi(argv[2]);
        arg.socketBufferSize = atoi(argv[4]);
        arg.cyclicBufferSize = atoi(argv[6]);
        sizeofbloom = atoi(argv[8]);
        arg.input_dir = malloc( sizeof(char) * (strlen(argv[10]) + 1) );
        strcpy(arg.input_dir,argv[10]);
        arg.numofthreads = atoi(argv[12]);
        

    } else {

        printf("input: %s %s %s %s %s %s\n", argv[1], argv[3], argv[5], argv[7], argv[9], argv[11]);
        printf("Wrong input! Use format: –m, -b, -c, -s, -i, -t\n");
        return -1;
    }


    /*
    printf("Size of Bloom: %d\n", sizeofbloom);
    printf("Number of Monitors: %d\n", arg.numMonitors);
    printf("socketBufferSize: %d\n", arg.socketBufferSize);
    printf("Directory for Input: %s\n", arg.input_dir);
    printf("cyclicBufferSize: %d\n", arg.cyclicBufferSize);
    printf("Number of Threads: %d\n\n", arg.numofthreads);
    */
    
    //Hashtable of Countries
    CountryMainHash countries;
    countries = hashtable_createCounMain();


    //SOCKET
    int port = 49155;


    //Δημιουργία monitor processes
    commun = malloc( sizeof(struct MonitorStruct)*arg.numMonitors); //allocation of communication struct
    for (int i = 0; i < arg.numMonitors; i++){
        commun[i].viruses = NULL;
        commun[i].list_of_countries = NULL;
        commun[i].sock = -1;
        commun[i].port = port+i;
        commun[i].numOfCountries = 0;
        commun[i].serverptr = (struct sockaddr*)&(commun[i].server);
    }
    


    //Distribution of folders
    printf("\nDistribution of folders\n");
    distribute_subdirs(arg.input_dir, commun, countries);


    for (int i=0; i<arg.numMonitors; i++){
        create_child(i);
    }



//////////////////////////////////////////

    // FIND THE ADDRESS OF THE SERVER
    struct hostent *rem;

    // find hostname
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    //printf("HOSTNAME: %s\n", hostname);



    if ( (rem = gethostbyname(hostname)) == NULL ){
        herror("ERROR: in getting host by name");
        exit(1);
    }


    // find IP
    //char *IPstr = inet_ntoa(*((struct in_addr*) rem->h_addr_list[0]));
    //printf("IP: %s\n", IPstr);




    for (int i=0; i<arg.numMonitors; i++){      //  for every monitorServer

        //Creation of Socket
        if ( ( commun[i].sock = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
            perror("ERROR: in creating a socket (client)");
            exit(1);
        }

        commun[i].server.sin_family = AF_INET;
        memcpy(&(commun[i].server.sin_addr), rem->h_addr, rem->h_length);
        commun[i].server.sin_port = htons(commun[i].port);


    }



    // START THE CONNECTIONS
    for (int i=0; i<arg.numMonitors; i++){      //  for every monitorServer
        
        do { } while ( connect(commun[i].sock, commun[i].serverptr, sizeof(commun[i].server)) < 0 );


    }




    //ΑΝΑΜΟΝΗ ΓΙΑ BLOOMFILTERS

    //για select:
    fd_set ready_set;
    fd_set current;

    //initialization
    FD_ZERO(&current);

    //add file descriptors to current set
    for (int i = 0; i < arg.numMonitors; i++){
        FD_SET(commun[i].sock, &current);       
    }


    for (int i = 0; i < arg.numMonitors; i++){      //for each monitor


        ready_set = current;

        if ( select(FD_SETSIZE, &ready_set, NULL, NULL, NULL) < 0 ){
            perror("ERROR in selecting ready file descriptors\n");
            exit(1);
        }

        for ( int j=0; j<FD_SETSIZE; j++){
            if ( FD_ISSET(j, &ready_set) ){ //found a ready fd

                if (get_bloomfilters(commun, arg.socketBufferSize, arg.numMonitors, j, 0) == -1){
                    perror("ERROR in getting bloomfilters");
                    exit(1);
                }
                
                FD_CLR(j, &current);    //remove file descriptor from set

                break;
            }

        }



    }



    
    //ΛΗΨΗ ΜΗΝΥΜΑΤΟΣ ΕΤΟΙΜΟΤΗΤΑΣ

    //initialization
    FD_ZERO(&current);

    //add file descriptors to current set
    for (int i = 0; i < arg.numMonitors; i++){
        FD_SET(commun[i].sock, &current);       
    }

    char *ready = NULL;

    for (int i = 0; i < arg.numMonitors; i++){

        ready_set = current;

        if ( select(FD_SETSIZE, &ready_set, NULL, NULL, NULL) < 0 ){
            perror("ERROR in selecting ready file descriptors\n");
            exit(1);
        }


        for ( int j=0; j<FD_SETSIZE; j++){
            if ( FD_ISSET(j, &ready_set) ){ //found a ready fd


                ready = get_message(j, arg.socketBufferSize);
                if ( strcmp(ready, "READY") != 0 ){
                    printf("Something Went Wrong!\n");
                } else {
                    printf("Monitor %d is ready!\n", commun[i].pid);
                }
                
                free(ready);


                FD_CLR(j, &current);    //remove file descriptor from set

                break;
            }

        }     


    }
    


    ////////////////////////////////////////////////

    // SIGNAL HANDLING
    static struct sigaction sa1;        //SIGINT  
    static struct sigaction sa2;        //SIGQUIT 
    static struct sigaction sa3;        //SIGCHLD

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
    console(commun, countries, arg.socketBufferSize);


    //////////////////////////////////

    //RELEASE OF MEMORY

    hashtable_destroyCounMain(countries);
    free(countries);
    free(arg.input_dir);

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

            //printf("    Opening Directory: %s\n", dsub_dir->d_name);

            // add countries to monitorServer[i]'s country list
            addinFilesList( &(commun[i].list_of_countries), dsub_dir->d_name);
            commun[i].numOfCountries++;

            i = (i + 1) % arg.numMonitors;

        }

    }

    printf("Finished Distribution\n\n");

    closedir(maindr);
}



