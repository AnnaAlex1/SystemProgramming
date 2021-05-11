#include "sh_pipes.h"
#include "virus.h"
#include "countries.h"

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


extern int size_in_bytes;




char* get_message_wrong(int fd, size_t buffersize){

    char *message = malloc(buffersize);

    int res = read( fd, message, buffersize );
    if ( res < 0 ){
        perror("ERROR: in reading from Named Pipe");
        return "error";
    } else if ( res == 0 ){
        return "DONE";
    } else {
        printf("READ result: %d\n", res);
    }
    
    
    //printf("Message from buffer: %s\n", message);
    
    return message;
    

}




/*
char* get_message(int fd, size_t buffersize){


    char *complete_mess;
    char *message;
    message = malloc( sizeof(char) * buffersize );

    char *cur_mes = malloc( sizeof(char) * buffersize );
    
    int i = 0;
    int res;

    while(1) {
        printf("Read no %d\n", i);
        res = read( fd, message, buffersize );
        if ( res < 0 ){
            perror("ERROR: in reading from Named Pipe");
            return "error";
        }
       
        printf("Message from buffer: %s\n", message);


        if ( i != 0){
            cur_mes = realloc(cur_mes, sizeof(char)* ( (i+1)*buffersize) );
        }
        memcpy( cur_mes + i*buffersize, message, buffersize);

        fflush(stdout);

        if ( message[buffersize-1] == '\0' ){
            break;
        }
        //if ( res == 0 ){
        //    fflush(stdout);
        //    break;
        //}
        i++;

    }

    complete_mess = malloc(sizeof(char) * (strlen(cur_mes)+1));
    memcpy(complete_mess, cur_mes, strlen(cur_mes)+1);

    printf("Complete Message: %s\n", complete_mess);
    return complete_mess;
}
*/





int sent_message_wrong(int fd, const void* message, size_t bufferSize){

    int res = write(fd, message, bufferSize);  
    if ( res == -1 ){
        perror("ERROR: in writing in Named Pipe");
        return -1;
    } else {
        printf("WRITE result: %d\n", res);
    }

    
    return 0;

}



/*
int sent_message(int fd, const void* message){


       
    int total_len = strlen(message)+1;
    int remainder_len = total_len;        //length of string still to be sent
    char remainder[bufferSize];
    
    int i=0;
    while ( remainder_len > 0){

        //printf("Write no %d\n", i);
        memcpy(remainder, message + total_len - remainder_len, remainder_len);
        
        if ( remainder_len < bufferSize ){

            for (int i=remainder_len; i<bufferSize; i++){
                remainder[i] = '\0';
            }
        }

        if ( write(fd, remainder, 12) == -1 ){
            perror("ERROR: in writing in Named Pipe");
            return -1;
        }
        //printf("Write no %d END\n", i);
        remainder_len = remainder_len - bufferSize;
        i++;
    }

    
    return 0;

}
*/



int sent_bloomfilters(int fd, struct List* virus_list, size_t buffersize){

    if (virus_list == NULL){
        printf("No BloomFilters to be sent\n");
        return -1;
    }


    struct List* virus_temp = virus_list;
    char *message = malloc(buffersize);

    while( virus_temp != NULL){

        //sent name of virus
        //printf("VirusTemp->name = %s\n", virus_temp->name);
        memcpy(message, virus_temp->name, strlen(virus_temp->name)+1);
        sent_message_wrong(fd, message, buffersize);       

        //sent bloomfilter for virus
        memcpy(message, virus_temp->vacc_bloom->array, size_in_bytes);
        
        //print_Bloom( *(virus_temp->vacc_bloom->array));
        //print_Bloom( (struct bloomfilter) *message);

        sent_message_wrong(fd, message, buffersize);
        
        virus_temp = virus_temp->next;
    }

    strcpy(message, "DONE");
    sent_message_wrong(fd, message, buffersize);

    free(message);

    return 0;
}




int get_bloomfilters(struct MonitorStruct *commun, size_t buffersize, int numMonitors){

    char *virusname; // = malloc(buffersize); 
    char *bloomfilter; // = malloc(buffersize); 


    for (int i = 0; i < numMonitors; i++){      //for each monitor
        

        while( 1 ){

            //get name of virus
            virusname = get_message_wrong(commun[i].fd_r,buffersize);

            if ( strcmp(virusname, "DONE") == 0 ){
                printf("Done getting Bloomfilters from Monitor %d\n", commun[i].pid);
                //free(virusname);
                break;
            } else if ( strcmp(virusname, "error") == 0 ){
                perror("ERROR in getting name of virus");
                return -1;
            }


            //get bloomfilter of virus
            bloomfilter = get_message_wrong(commun[i].fd_r,buffersize);

            /*if ( strcmp(bloomfilter, "error") == 0 ){
                perror("ERROR in getting Bloomfilter of virus");
                return -1;
            }*/

            //printf("VirusName: %s\n", virusname);

            addinVirMain(&(commun[i].viruses), virusname, (unsigned int *)bloomfilter);

            free(virusname);
            free(bloomfilter);
        }



    }
    
    printVirMain(commun[0].viruses);
    print_Bloom(*(commun[0].viruses->vacc_bloom));

    
    return 0;
   

}