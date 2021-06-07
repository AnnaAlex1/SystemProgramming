#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>

#include "citizens.h"
#include "virus.h"
#include "countries.h"
#include "records.h"
#include "sh_sockets.h"
#include "console.h"
#include "signal_handl.h"
#include "threads_imp.h"



int socketBufferSize;
extern int sizeofbloom;

extern int signal_num;


void monitor_body(int sock, Hashtable citizens, struct List **viruslist, CountryHash countries);






int main(int argc, char* argv[]){

    printf("Hello from Monitor pid:%d!\n", getpid());
    //printf("Number of Arguments: %d\n", argc);


    //GET ARGUMENTS
    int port = atoi(argv[2]);
    int numThreads = atoi(argv[4]);
    socketBufferSize = atoi(argv[6]);
    int cyclicBufferSize = atoi(argv[8]);
    sizeofbloom = atoi(argv[10]);

    
    /*printf("\nMONITOR ARGUMENTS:\n");
    printf("Port: %d\n", port);
    printf("Number of Threads: %d\n", numThreads);
    printf("Socket Buffersize: %d\n", socketBufferSize);
    printf("CyclicBuffersize: %d\n", cyclicBufferSize);
    printf("Sizeofbloom: %d\n", sizeofbloom);*/
    

    

    // STORE RECORDS FROM COUNTRY FOLDERS
    //List of viruses
    struct List* virus_list = NULL;

    //Hashtable of citizens
    Hashtable citizens;
    citizens = hashtable_create();

    //Hashtable of Countries
    CountryHash countries;
    countries = hashtable_createCoun();

    for (int i = 11; i < argc; i++){        //getting countries from arguments
        hashtable_addCoun(countries, argv[i]);
    }
    
    


    //ΑΡΧΙΚΟΠΟΙΗΣΗ: Αποθήκευση εγγραφών από νήματα
    // THREADS STRUCT
    pthread_t *my_thr;      //array of threads
    my_thr = malloc( sizeof(pthread_t) * numThreads);

    if ( thread_store_records(my_thr, numThreads, cyclicBufferSize, argc-11, citizens, &virus_list, countries) == -1 ){
        printf("ERROR in storing records");
        exit(1);
    }




    //SOCKET
    int sock, newsock;

    struct sockaddr_in server, client;
    socklen_t clientlen;
    
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    

    // Creation of Socket
    if ( (sock = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        perror("ERROR: in creating a Socket (server)");
        exit(1);
    }

    // to reuse
    int reuse_Addr = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_Addr, sizeof(reuse_Addr));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);



    //  Binding socket to the address
    if ( bind(sock, serverptr, sizeof(server)) < 0 ){
        perror("ERROR: during binding");
        exit(1);
    }

    // Listening for connections
    if ( listen(sock, 1) < 0 ){
        perror("ERROR: during listening");
        exit(1);
    }
    printf("Listening for connections to port %d from monitor %d\n", port, getpid());


    // Accepting a connection
    clientlen = sizeof(client);
    if ( (newsock = accept(sock, clientptr, &clientlen)) < 0 ){
        perror("ERROR: during accepting of connection");
        exit(1);
    }

    printf("MonitorServer %d accepted Connection\n", getpid());


    //ΑΠΟΣΤΟΛΗ ΤΩΝ Bloomfilter (για κάθε ίωση)
    if (send_bloomfilters(newsock, virus_list, socketBufferSize) == -1){
        perror("ERROR in sending bloomfilters");
        exit(1);
    }
        

    //ΑΠΟΣΤΟΛΗ ΜΗΝΥΜΑΤΟΣ ΕΤΟΙΜΟΤΗΤΑΣ ΓΙΑ ΑΙΤΗΜΑΤΑ
    send_message(newsock, "READY", strlen("READY")+1, socketBufferSize);




    ////////////////////////////////
    //SIGNAL HANDLING

    static struct sigaction sa1;        //SIGINT
    static struct sigaction sa2;        //SIGQUIT
    static struct sigaction sa3;        //SIGUSR1
    static struct sigaction sa4;

    sa1.sa_handler = handle_MonitorFin;
    sigfillset (&(sa1.sa_mask));
    sigaction(SIGINT , &sa1 , NULL);

    sa2.sa_handler = handle_MonitorFin;
    sigfillset (&(sa2.sa_mask));
    sigaction(SIGQUIT , &sa2 , NULL);

    sa3.sa_handler = handle_newfiles;
    sigfillset (&(sa3.sa_mask));
    sigaction(SIGUSR1 , &sa3 , NULL);
    
    sa4.sa_handler = handle_needtoread;
    sigfillset (&(sa4.sa_mask));
    sigaction(SIGUSR2 , &sa4 , NULL);   

    
    //////////////////////////////////
    //MAIN FUNCTION OF MONITORS
    monitor_body(newsock, citizens, &virus_list, countries);



    //////////////////////////////////////////////////

    //PRINT STRUCTURES
    /*printf("\n\nPRINT STRUCTURES: %s\n", argv[1]);
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries, stdout);*/
    

    //RELEASE OF MEMORY
    printf("\nRELEASE OF MEMORY.\n");

    thread_finish(numThreads, cyclicBufferSize, my_thr);
    free(my_thr);

    close(newsock);
    close(sock);

    hashtable_destroyCoun(countries);
    hashtable_destroy(citizens);
    deletelist(&virus_list);
    free(citizens);
    free(countries);
    
    printf("Monitor pid:%d exiting...\n", getpid());

    return 0;
}




void monitor_body(int sock, Hashtable citizens, struct List **viruslist, CountryHash countries){


    int total_requests = 0;
    int accepted_req = 0;
    int rejected_req = 0;

    char *command;

    printf("\nMonitor %d Waiting For Instructions\n", getpid());

    while( 1 ){


        if ( signal_num == 5){      // SIGNAL FOR READING

            //GET CHOSEN COMMAND
            command = get_message(sock, socketBufferSize);

            if ( strcmp(command, "travelRequest") == 0 ){

                total_requests++;

                //GET NECESSARY INFO
                //get name of virus
                char *virus = get_message(sock, socketBufferSize);
                //get citizenID
                char *citizenID = get_message(sock, socketBufferSize);


                //get struct for virus
                struct List* virusNode = getelemfromlist(*viruslist, virus);
                if ( virusNode == NULL ){
                    printf("No such Virus!\n");
                    send_message(sock, "Error", strlen("Error")+1, socketBufferSize);
                    continue;
                }

                //try to get a date of vaccination
                char *date;
                if ( (date = getDate_VacSkipList(virusNode->vaccinated, citizenID) ) != NULL ){
                    //found
                    printf("VACCINATED ON %s\n", date);
                    
                    //send answer "YES"
                    send_message(sock, "YES", strlen("YES")+1, socketBufferSize);

                    //send date
                    send_message(sock, date, strlen(date)+1, socketBufferSize);

                    accepted_req++;

                } else {    //no record for this person
                    printf("NOT VACCINATED\n");

                    //send answer "NO"
                    send_message(sock, "NO", strlen("NO")+1, socketBufferSize);

                    rejected_req++;
                }



            } else if ( strcmp(command, "searchVaccinationStatus") == 0 ){

                char *citizenID = get_message(sock, socketBufferSize);

                struct Citizen *cit = hashtable_get(citizens, citizenID);

                if ( cit == NULL ){     //citizen not in this monitor
                    
                    send_message(sock, "NOT FOUND", strlen("NOT FOUND")+1, socketBufferSize);

                } else {        //citizen found

                    send_message(sock, "FOUND", strlen("FOUND")+1, socketBufferSize);

                    //send info
                    char info[300];
                    sprintf(info, "%s %s %s %s", cit->citizenID, cit->firstname, cit->lastname, cit->country);

                    //send citizen details
                    send_message(sock, info, strlen(info)+1, socketBufferSize);

                    //send age
                    sprintf(info, "%d", cit->age);
                    send_message(sock, info, strlen(info)+1, socketBufferSize );


                    struct List *temp = *viruslist;
                    char *vac_date;
                    

                    while ( temp != NULL){  //for every virus

                        vac_date = getDate_VacSkipList( temp->vaccinated, citizenID );

                        if ( vac_date == NULL ){    //not yet vaccinated
                            
                            sprintf(info,"%s %s", temp->name, " NOT YET VACCINATED");

                        } else {        //vaccinated

                            sprintf(info,"%s %s %s", temp->name, " VACCINATED ON ", vac_date);

                        }

                        send_message(sock, info, strlen(info)+1, socketBufferSize);
                        

                        temp = temp->next;
                    }

                    //send "DONE" message
                    send_message(sock, "DONE", strlen("DONE")+1, socketBufferSize);

                }


                free(citizenID);

            } else if ( strcmp(command, "addVaccinationRecords") == 0 ) {
            
                //ΔΙΑΒΑΣΕ ΝΕΑ ΑΡΧΕΙΑ
                //Γέμισε τις δομές

                if ( fill_buffer_new_files(citizens, viruslist, countries, socketBufferSize) == -1){
                    printf("ERROR in Reading new Files\n");
                }

                printf("Finished with reading new Files!\n");

                //Αποστολή των Bloomfilter (για κάθε ίωση)
                if (send_bloomfilters(sock, *viruslist, socketBufferSize) == -1){
                    perror("ERROR in sending bloomfilters");
                    exit(1);
                }



            }


            signal_num = 0;


        } else if ( signal_num == 3 ){      //GOT A SIGINT/SIGQUIT SIGNAL

            //build log_file's name            
            int pid = getpid();
            char pid_str[7];
            sprintf(pid_str, "%d", pid);

            char *filename = malloc( sizeof(char) * (strlen("logfiles/log_file.")+strlen(pid_str)+1) );
            strcpy(filename, "logfiles/log_file.");
            strcat(filename, pid_str);

            FILE* logfile = fopen(filename, "w");
            if ( logfile == NULL){
                perror("ERROR in opening file 'LOGFILE'");
                return;
            }

            //print info in log_file
            print_hashtableCoun(countries, logfile);
            fprintf(logfile, "%d\n%d\n%d", total_requests, accepted_req, rejected_req);
            
            fclose(logfile);
            free(filename);

            signal_num = 0; //reset signal number
            
            return;


        } 




    }



}

