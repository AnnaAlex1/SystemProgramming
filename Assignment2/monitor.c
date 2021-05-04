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

int get_message(int fd, char *complete_mess, int buffersize);


int main(int argc, char* argv[]){

    //check for right number of arguments

    //printf("Hello from Monitor (from argument) %s!\n", argv[1]);

    char *message;

    //int buffersize;


    // OPEN NAMED PIPE
    //printf("Argument 1: %s\n", argv[1]);
    int fd = open(argv[1], O_RDONLY);
    if ( fd < 0 ){
        perror("ERROR: in opening Named Pipe (from Monitor)\n");
        exit(1);
    }

    //Waiting for BufferSize
    //message = malloc(sizeof(char)* sizeof(int));     //initial value to get buffersize
    

    /*if (get_message( fd, message, sizeof(int)) == -1){
        exit(2);
    }*/

    ///////////////////////////////////////????


    close(fd);

    //////////////////////////

    //printf("This is the message: %c\n", c);

    //buffersize = atoi(message);

    //free(message);


    //ΑΡΧΙΚΟΠΟΙΗΣΗ: Αναμονή για χώρες
    //διαβάζει μέσω των named pipes τις χώρες που θα αναλάβει

    //get_message();


    //Αναμονή για Bloomfilter

    //printf("Hello from Monitor (from pipe) %s!\n", argv[1]);


    printf("HEREEEE\n");

    //close(fd);





    return 0;
}





int get_message(int fd, char *complete_mess, int buffersize){


    
    char *message;
    message = malloc( sizeof(char) * buffersize );

    char *cur_mes = malloc( sizeof(char) * buffersize );
    
    int i = 0;

    while(1) {
        //printf("Read no %d\n", i);

        if ( read( fd, message, 12 ) < 0 ){
            perror("ERROR: in reading from Named Pipe");
            return -1;
        }
       
        printf("Message from buffer: %s\n", message);

        if ( message[10] == '\0' ){
            break;
        }

        if ( i != 0){
            cur_mes = realloc(cur_mes, sizeof(char)* ( (i+1)*buffersize) );
        }
        memcpy( cur_mes + i*buffersize, message, buffersize);

        fflush(stdout);
        i++;

    }

    complete_mess = malloc(sizeof(char) * (strlen(cur_mes)+1));
    memcpy(complete_mess, cur_mes, strlen(cur_mes)+1);

    printf("Complete Message: %s\n", complete_mess);
    return 0;
}