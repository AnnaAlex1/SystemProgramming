#include "sh_pipes.h"
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


extern int size_in_bytes;
extern struct MonitorStruct *commun;
int numMonitors;



void create_child(int i, int buffersize){
    
    //Δημιουργία named pipes
    char w_pname[] = "pipes/wfifo";
    char r_pname[] = "pipes/rfifo";
    char *extw_pname;
    char *extr_pname;
    char number[5];



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
            exit(1);
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



    char mes_to_send[10];
    sprintf(mes_to_send, "%d", buffersize);
    //printf("MAIN PROGRAM to send buffersize: %s\n", mes_to_send);

    //SEND BUFFERSIZE
    send_message(commun[i].fd_w, mes_to_send, strlen(mes_to_send)+1, sizeof(long));

    //SEND BLOOMSIZE
    sprintf(mes_to_send, "%d", size_in_bytes);
    //printf("MAIN PROGRAM to send bloomsize: %s\n", mes_to_send);
    send_message(commun[i].fd_w, mes_to_send, strlen(mes_to_send)+1, buffersize);


    free(extw_pname);
    free(extr_pname);

}









char* get_message(int fd, size_t buffersize){


    //char *complete_mess;
    char *message;
    message = malloc( buffersize );

    char *cur_mes = malloc( buffersize );
    
    int res;
    int remainder_len;


    //get size of message
    res = read(fd, message, buffersize);
    if ( res < 0 ){
        perror("ERROR: in reading from Named Pipe (size of message)");
        return NULL;
    }
    int total_message = atoi(message);
    //printf("Size of message to receive: %d\n", total_message);


    //get number of rounds to perform
    int rounds = ceil( (double)total_message / (double)buffersize );
    //printf("GET MESSAGE: Rounds: %d\n", rounds);



    for (int i = 0; i < rounds; i++){
    

        //printf("Read no %d\n", i);

        //read i-th part of message
        res = read( fd, message, buffersize );
        if ( res < 0 ){
            perror("ERROR: in reading from Named Pipe");
            return NULL;
        }

        if ( i == rounds-1 ){       //for the last round
            
            
            remainder_len = total_message % buffersize;
            if (remainder_len == 0){
                remainder_len = buffersize;
            }
            
            //allocate more space for new info
            //printf("Realloc size to realloc: %ld\n",  i*buffersize + remainder_len);
            cur_mes = realloc(cur_mes, i*buffersize + remainder_len );
            
            //printf("Remainder length=%d\n",remainder_len);

            memcpy(cur_mes + i*buffersize, message, remainder_len);

        } else{
            
            if ( i != 0 ){ //not first round
                cur_mes = realloc(cur_mes, (i+1)*buffersize);
            }
            //printf("Realloc size to realloc: %ld\n",  (i+1)*buffersize);

            memcpy(cur_mes + i*buffersize, message, buffersize);

        }

        

    }


    free(message);

    return cur_mes;
}








int send_message(int fd, const void* message, int size_of_message, size_t buffersize){


    int remainder_len;        //length of string for last round
    char to_send[buffersize];
    

    //get number of rounds to perform
    int rounds = ceil( (double)size_of_message / (double)buffersize );
    //printf("SEND MESSAGE Rounds: %d\n", rounds);  

    //send message's size
    char size_str[10];
    sprintf(size_str, "%d", size_of_message);
    if ( write(fd, size_str, buffersize) == -1 ){
        perror("ERROR: in writing in Named Pipe (size of message)");
        return -1;
    }

    //printf("Size of message to send: %d     %s\n", size_of_message, size_str);

    for( int i=0; i<rounds; i++ ){

        if ( i == rounds-1 ){   //if last round
            remainder_len = size_of_message % buffersize;
            if (remainder_len == 0){
                remainder_len = buffersize;
            }
            
            //printf("Remainder length=%d\n",remainder_len);

            memcpy(to_send, message + i*buffersize, remainder_len);

        } else {        //every other round
            memcpy(to_send, message + i*buffersize, buffersize);
        }
        
        

        if ( write(fd, to_send, buffersize) == -1 ){
            perror("ERROR: in writing in Named Pipe");
            return -1;
        }

        
    }

    
    return 0;

}











int send_bloomfilters(int fd, struct List* virus_list, size_t buffersize){

    if (virus_list == NULL){
        printf("No BloomFilters to be send\n");
        return -1;
    }

    struct List* virus_temp = virus_list;

    while( virus_temp != NULL){

        //send name of virus
        //printf("SENDD: VirusTemp->name = %s\n", virus_temp->name);
        send_message(fd, virus_temp->name, strlen(virus_temp->name)+1, buffersize);       

        //print_Bloom( *(virus_temp->vacc_bloom->array));
        //print_Bloom( (struct bloomfilter) *message);

        //send bloomfilter for virus
        send_message(fd, virus_temp->vacc_bloom->array, size_in_bytes+sizeof(int), buffersize);
        
        virus_temp = virus_temp->next;
    }

    send_message(fd, "DONE", strlen("DONE")+1, buffersize);

    return 0;
}









int get_bloomfilters(struct MonitorStruct *commun, size_t buffersize, int numMonitors){

    char *virusname = NULL; // = malloc(buffersize); 
    char *bloomfilter; // = malloc(buffersize); 
    unsigned int *bloomf;
    bloomf = malloc( size_in_bytes + sizeof(int));



    for (int i = 0; i < numMonitors; i++){      //for each monitor
        

        while( 1 ){

            //get name of virus
            virusname = get_message(commun[i].fd_r, buffersize);
            printf("GOT: virusname: %s\n", virusname);

            if ( virusname == NULL ){
                perror("ERROR in getting name of virus");
                return -1;
            } else if ( strcmp(virusname, "ending") == 0){
                printf("ERROR2 in getting name of virus\n");
                return -1;
            }

            //printf("LAST: Message from buffer: %s\n", virusname);
            if ( strcmp(virusname, "DONE") == 0 ){
                printf("Done getting Bloomfilters from Monitor %d\n", commun[i].pid);
                //free(virusname);
                break;
            }

            //get bloomfilter of virus
            bloomfilter = get_message(commun[i].fd_r,buffersize);

            memcpy(bloomf, bloomfilter, size_in_bytes + sizeof(int));

            
            //Add Viruses and corresponding Bloomfilters to list
            addinVirMain(&(commun[i].viruses), virusname, bloomf);
            //print_Bloom(*(commun[i].viruses->vacc_bloom));
            

            free(virusname);
            virusname = NULL;
            free(bloomfilter);
            
        }



    }
    
   
    return 0;
   

}


