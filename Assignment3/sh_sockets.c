#include "sh_sockets.h"
#include "virus.h"
#include "countries.h"
#include "bloomfilter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <math.h>



extern struct MonitorStruct *commun;
struct Arguments arg;
extern int sizeofbloom;

void create_child(int i){
    
    

    //FORK CHILD PROCESS
    commun[i].pid = fork();

    if ( commun[i].pid == -1 ){
        perror("ERROR: in fork");
        exit(1);
    }

    if ( commun[i].pid == 0 ){
        //this is a child process

        char *args[] = {"./monitorServer", "-p", commun[i].port, "-t", arg.numofthreads, "-b", 
        arg.socketBufferSize, "-c", arg.cyclicBufferSize, "-s", sizeofbloom, 
        commun[i].list_of_countries,  NULL};
        
        /*
        monitorServer -p port -t numThreads -b socketBufferSize -c cyclicBufferSize -s 
        sizeOfBloom path1 path2 ... pathn
        */

        //execute Monitor Process
        if (execv(args[0], args)){
            perror("ERROR: in exec");
            exit(1);
        }
    }




    char mes_to_send[10];

    //SEND socketBufferSize
    sprintf(mes_to_send, "%d", arg.socketBufferSize);
    send_message(commun[i].sock, mes_to_send, strlen(mes_to_send)+1, sizeof(long));

    //SEND BLOOMSIZE
    sprintf(mes_to_send, "%d", sizeofbloom);
    //printf("MAIN PROGRAM to send bloomsize: %s\n", mes_to_send);
    send_message(commun[i].sock, mes_to_send, strlen(mes_to_send)+1, arg.socketBufferSize);



}









char* get_message(int fd, size_t socketBufferSize){


    char *message;
    message = malloc( socketBufferSize );

    char *cur_mes = malloc( socketBufferSize );
    
    int res;
    int remainder_len;


    //get size of message
    res = read(fd, message, socketBufferSize);
    if ( res < 0 ){
        perror("ERROR: in reading from Named Pipe (size of message)");
        return NULL;
    }
    int total_message = atoi(message);
    //printf("Size of message to receive: %d\n", total_message);


    //get number of rounds to perform
    int rounds = ceil( (double)total_message / (double)socketBufferSize );


    for (int i = 0; i < rounds; i++){
    
        //read i-th part of message
        res = read( fd, message, socketBufferSize );
        if ( res < 0 ){
            perror("ERROR: in reading from Named Pipe");
            return NULL;
        }

        if ( i == rounds-1 ){       //for the last round
            
            
            remainder_len = total_message % socketBufferSize;
            if (remainder_len == 0){
                remainder_len = socketBufferSize;
            }
            
            //allocate more space for new info
            cur_mes = realloc(cur_mes, i*socketBufferSize + remainder_len );
            
            memcpy(cur_mes + i*socketBufferSize, message, remainder_len);

        } else{
            
            if ( i != 0 ){ //not first round
                cur_mes = realloc(cur_mes, (i+1)*socketBufferSize);
            }

            memcpy(cur_mes + i*socketBufferSize, message, socketBufferSize);

        }

        

    }


    free(message);

    return cur_mes;
}








int send_message(int fd, const void* message, int size_of_message, size_t socketBufferSize){


    int remainder_len;        //length of string for last round
    char to_send[socketBufferSize];
    

    //get number of rounds to perform
    int rounds = ceil( (double)size_of_message / (double)socketBufferSize );

    //send message's size
    char size_str[10];
    sprintf(size_str, "%d", size_of_message);
    
    if ( write(fd, size_str, socketBufferSize) == -1 ){
        perror("ERROR: in writing in Named Pipe (size of message)");
        return -1;
    }


    for( int i=0; i<rounds; i++ ){

        if ( i == rounds-1 ){   //if last round
            remainder_len = size_of_message % socketBufferSize;
            if (remainder_len == 0){
                remainder_len = socketBufferSize;
            }
            
            //printf("Remainder length=%d\n",remainder_len);

            memcpy(to_send, message + i*socketBufferSize, remainder_len);

        } else {        //every other round
            memcpy(to_send, message + i*socketBufferSize, socketBufferSize);
        }
        
        

        if ( write(fd, to_send, socketBufferSize) == -1 ){
            perror("ERROR: in writing in Named Pipe");
            return -1;
        }

        
    }

    
    return 0;

}











int send_bloomfilters(int fd, struct List* virus_list, size_t socketBufferSize){

    if (virus_list == NULL){
        printf("No BloomFilters to be send\n");
        return -1;
    }

    struct List* virus_temp = virus_list;

    while( virus_temp != NULL){

        //send name of virus
        send_message(fd, virus_temp->name, strlen(virus_temp->name)+1, socketBufferSize);       

        //send bloomfilter for virus
        send_message(fd, virus_temp->vacc_bloom->array, sizeofbloom+sizeof(int), socketBufferSize);
        
        virus_temp = virus_temp->next;
    }

    send_message(fd, "DONE", strlen("DONE")+1, socketBufferSize);

    return 0;
}









int get_bloomfilters(struct MonitorStruct *commun, size_t socketBufferSize, int numMonitors, int i, int replace){

    char *virusname = NULL;
    char *bloomfilter;  
    unsigned int *bloomf;
    bloomf = malloc( sizeofbloom + sizeof(int));


    int pos_in_commun;

    if ( !replace ){    //case: first read of bloomfilter, 'i' represents the file_des itself

        //find the position in commun struct
        for (int z = 0; z < numMonitors; z++){
            if ( commun[z].sock == i ){
                pos_in_commun = z;
            }
        }

    } else {
        pos_in_commun = i;
    }


    while( 1 ){

        //get name of virus
        virusname = get_message(commun[pos_in_commun].sock, socketBufferSize);

        if ( virusname == NULL ){
            perror("ERROR in getting name of virus");
            return -1;
        }

        if ( strcmp(virusname, "DONE") == 0 ){
            printf("Done getting Bloomfilters from Monitor %d\n", commun[pos_in_commun].pid);
            free(virusname);
            break;
        }

        //get bloomfilter of virus
        bloomfilter = get_message(commun[pos_in_commun].sock,socketBufferSize);

        memcpy(bloomf, bloomfilter, sizeofbloom + sizeof(int));

        
        if (replace){   //if there is an older version of the bloomfilter

            replace_bloom(commun[pos_in_commun].viruses, virusname, bloomf);

        } else {    //initial send of bloomfilter
            
            //Add Viruses and corresponding Bloomfilters to list
            addinVirMain(&(commun[pos_in_commun].viruses), virusname, bloomf);
        }
              

        free(virusname);
        virusname = NULL;
        free(bloomfilter);
        
    }


    free(bloomf);
    return 0;
   

}


