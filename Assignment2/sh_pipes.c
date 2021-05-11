#include "sh_pipes.h"

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



char* get_message_wrong(int fd, size_t buffersize){

    char *message = malloc(buffersize );

    int res = read( fd, message, buffersize );
    if ( res < 0 ){
        perror("ERROR: in reading from Named Pipe");
        return "error";
    } else {
        printf("READ result: %d\n", res);
    }
    
    
    printf("Message from buffer: %s\n", message);
    
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