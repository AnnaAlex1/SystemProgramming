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

//char* get_message(int fd, ssize_t buffersize);
char* get_message_wrong(int fd, ssize_t buffersize);


int main(int argc, char* argv[]){

    printf("Hello from Monitor with pipe %s!\n", argv[1]);


    //check for right number of arguments
    if (argc != 2){
        perror("Wrong number of arguments\n!");
    }


    // OPEN NAMED PIPE
    int fd = open(argv[1], O_RDONLY);
    if ( fd < 0 ){
        perror("ERROR: in opening Named Pipe (from Monitor)\n");
        exit(1);
    }

    //GET BUFFERSIZE    
    char *message;
    message = get_message_wrong( fd, 10);         //get buffersize
    if ( strcmp(message, "error") == 0){
        exit(2);
    }

    ssize_t buffersize;

    buffersize = atoi(message);
    printf("Buffersize: %ld\n", buffersize);

    free(message);
    ///////////////////////////////////////????




    //ΑΡΧΙΚΟΠΟΙΗΣΗ: Αναμονή για χώρες
    //διαβάζει μέσω των named pipes τις χώρες που θα αναλάβει
    while(1) {
        message = get_message_wrong(fd, buffersize);
        if ( strcmp(message, "DONE") != 0 ){
            free(message);
            break;
        }

        printf("Monitor %s gets directory: %s\n", argv[1], message);
        //memcpy();
        free(message);
        

    }


    //Αναμονή για Bloomfilter

    //printf("Hello from Monitor (from pipe) %s!\n", argv[1]);


    printf("\nHER77777777777777777777777777777777EEEE\n");

    
    close(fd);


    return 0;
}



char* get_message_wrong(int fd, ssize_t buffersize){

    char *message = malloc( sizeof(char) * buffersize );

    int res = read( fd, message, 10000 );
    if ( res < 0 ){
        perror("ERROR: in reading from Named Pipe");
        return "error";
    }
    
    
    printf("Message from buffer: %s\n", message);
    fflush(stdout);
    
    return message;
    

}




/*
char* get_message(int fd, ssize_t buffersize){


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