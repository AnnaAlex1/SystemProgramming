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


#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>



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




    struct CountryFiles *temp = commun[i].list_of_countries;



    if ( commun[i].pid == 0 ){  //this is a child process
        


        int args_size = commun[i].numOfCountries + 12;
        char *args[args_size];

        //adding arguments
        args[0] = malloc( sizeof(char) * (strlen("./monitorServer")+1) );       // monitorServer
        strcpy(args[0], "./monitorServer");
        args[1] = malloc( sizeof(char) * (strlen("-p")+1) );                    // -p
        strcpy(args[1], "-p");
        args[2] = malloc( sizeof(char) * 10 );
        sprintf(args[2], "%d", commun[i].port);                                 // port
        args[3] = malloc( sizeof(char) * (strlen("-t")+1) );
        strcpy(args[3], "-t");                                                  // -t
        args[4] = malloc( sizeof(char) * 10 );
        sprintf(args[4], "%d", arg.numofthreads);                               // numofthreads
        args[5] = malloc( sizeof(char) * (strlen("-b")+1) );
        strcpy(args[5], "-b");                                                  // -b
        args[6] = malloc( sizeof(char) * 10 );
        sprintf(args[6], "%d", arg.socketBufferSize);                           // socketBufferSize 
        args[7] = malloc( sizeof(char) * (strlen("-c")+1) );
        strcpy(args[7], "-c");                                                  // -c
        args[8] = malloc( sizeof(char) * 10 );
        sprintf(args[8], "%d", arg.cyclicBufferSize);                           // cyclicBufferSize
        args[9] = malloc( sizeof(char) * (strlen("-s")+1) );
        strcpy(args[9], "-s");                                                  // -s
        args[10] = malloc( sizeof(char) * 10 );
        sprintf(args[10], "%d", sizeofbloom);                                   // sizeofbloom     


        //adding filenames
        for (int j = 11; j < args_size - 1; j++){
            
            args[j] = malloc( sizeof(char) * (strlen(temp->filename)+1) );
            strcpy(args[j], temp->filename);

            temp = temp->next;

        }
        
        //adding NULL
        args[args_size-1] = NULL; 

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



}









char* get_message(int fd, int socketBufferSize){


    char *message;
    message = malloc( socketBufferSize );

    char *cur_mes = malloc( socketBufferSize );
    
    int res;
    int remainder_len;


    //get size of message
    res = recv(fd, message, socketBufferSize, MSG_WAITALL);
    if ( res < 0 ){
        perror("ERROR: in reading from Socket (size of message)");
        return NULL;
    }
    int total_message = atoi(message);
    printf("Size of message to receive: %d\n", total_message);


    //get number of rounds to perform
    int rounds = ceil( (double)total_message / (double)socketBufferSize );


    for (int i = 0; i < rounds; i++){
    
        //read i-th part of message
        res = recv( fd, message, socketBufferSize, MSG_WAITALL);
        if ( res < 0 ){
            perror("ERROR: in reading from Socket");
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

/*

char* get_message(int fd, int socketBufferSize){


    char *message;
    message = malloc( socketBufferSize);
    
    int res;
    int remainder_len;


    //get size of message
    res = read(fd, message, socketBufferSize);
    if ( res < 0 ){
        perror("ERROR: in reading from Socket (size of message)");
        return NULL;
    }
    int total_message = atoi(message);
    //printf("Size of message to receive: %d\n", total_message);

    char *cur_mes = malloc( total_message );

    //get number of rounds to perform
    int rounds = ceil( (double)total_message / (double)socketBufferSize );


    for (int i = 0; i < rounds; i++){
    
        //read i-th part of message
        res = read( fd, message, socketBufferSize );
        if ( res < 0 ){
            perror("ERROR: in reading from Socket");
            return NULL;
        }

        if ( i == rounds-1 ){       //for the last round
            
            
            remainder_len = total_message % socketBufferSize;
            if (remainder_len == 0){
                remainder_len = socketBufferSize;
            }
            
            //allocate more space for new info
            //cur_mes = realloc(cur_mes, i*socketBufferSize + remainder_len );
            
            memcpy(cur_mes + i*socketBufferSize, message, remainder_len);

        } else{
            
            //if ( i != 0 ){ //not first round
             //   cur_mes = realloc(cur_mes, (i+1)*socketBufferSize);
            //}

            memcpy(cur_mes + i*socketBufferSize, message, socketBufferSize);

        }

        

    }


    free(message);

    return cur_mes;
}


*/




int send_message(int fd, const void* message, int size_of_message, int socketBufferSize){


    int remainder_len;        //length of string for last round
    char to_send[socketBufferSize];
    memset(to_send, 0, socketBufferSize);


    //get number of rounds to perform
    int rounds = ceil( (double)size_of_message / (double)socketBufferSize );

    //send message's size
    char size_str[10];
    memset(size_str, 0, sizeof(size_str));
    sprintf(size_str, "%d", size_of_message);
    
    if ( write(fd, size_str, socketBufferSize) == -1 ){
        perror("ERROR: in writing in Socket (size of message)");
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
            perror("ERROR: in writing in Socket");
            return -1;
        }

        
    }

    
    return 0;

}











int send_bloomfilters(int fd, struct List* virus_list, int socketBufferSize){

    if (virus_list == NULL){
        printf("No BloomFilters to be send\n");
        return -1;
    }

    struct List* virus_temp = virus_list;

    while( virus_temp != NULL){

        //send name of virus
        printf("To SEND: %s         from %d with socketbuffersize of %d\n", virus_temp->name, getpid(), socketBufferSize);
        send_message(fd, virus_temp->name, strlen(virus_temp->name)+1, socketBufferSize);       

        //send bloomfilter for virus
        send_message(fd, virus_temp->vacc_bloom->array, sizeofbloom+sizeof(int), socketBufferSize);
        
        virus_temp = virus_temp->next;
    }

    send_message(fd, "DONE", strlen("DONE")+1, socketBufferSize);

    return 0;
}









int get_bloomfilters(struct MonitorStruct *commun, int socketBufferSize, int numMonitors, int i, int replace){

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

    printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW  pid: %d   sock: %d    port: %d", commun[pos_in_commun].pid, commun[pos_in_commun].sock, commun[pos_in_commun].port);
    printf("   SocketBufferSize: %d  %d\n", socketBufferSize, arg.socketBufferSize);
    
    while( 1 ){

        //get name of virus
        virusname = get_message(commun[pos_in_commun].sock, socketBufferSize);
        printf("Virusname: %s\n", virusname);

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
        bloomfilter = get_message(commun[pos_in_commun].sock, socketBufferSize);

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




char* get_mes_client(int sock, struct sockaddr_in server, struct sockaddr *serverptr ){

    char *mes;

    // START A CONNECTION
    if ( connect(sock, serverptr, sizeof(server)) < 0 ){
        perror("ERROR: in connection");
        exit(1);
    }

    mes = get_message(sock, arg.socketBufferSize);

    close(sock);

    return mes;

}


void send_mes_server(int sock, struct sockaddr *clientptr, socklen_t clientlen, const void* message, int size_of_message, int socketBufferSize){

    int newsock;

    // Accepting a connection
    if ( (newsock = accept(sock, clientptr, &clientlen)) < 0 ){
        perror("ERROR: during accepting of connection");
        exit(1);
    }

    printf("Accepted Connection\n");
    close(sock); 
    
    send_message(newsock, message, size_of_message, socketBufferSize);

    
    close(newsock);

}