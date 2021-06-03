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
#include "citizens.h"
#include "countries.h"


size_t socketBufferSize;
extern int sizeofbloom;

extern int signal_num;


int store_records(Hashtable citizens, struct List** virus_list, CountryHash countries);
void monitor_body(int sock, Hashtable citizens, struct List **viruslist, CountryHash countries);
int read_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries);






int main(int argc, char* argv[]){

    printf("Hello from Monitor pid:%d!\n", getpid());
    printf("Number of Arguments: %d\n", argc);


    //GET ARGUMENTS
    int port = atoi(argv[2]);
    int numThreads = atoi(argv[4]);
    socketBufferSize = atoi(argv[6]);
    int cyclicBufferSize = atoi(argv[8]);
    sizeofbloom = atoi(argv[10]);


    printf("\nMONITOR ARGUMENTS:\n");
    printf("Port: %d\n", port);
    printf("Number of Threads: %d\n", numThreads);
    printf("Socket Buffersize: %ld\n", socketBufferSize);
    printf("CyclicBuffersize: %d\n", cyclicBufferSize);
    printf("Sizeofbloom: %d\n", sizeofbloom);


    

    // STORE RECORDS FROM COUNTRY FOLDERS
    //List of viruses
    struct List* virus_list = NULL;

    //Hashtable of citizens
    Hashtable citizens;
    citizens = hashtable_create();

    //Hashtable of Countries
    CountryHash countries;
    countries = hashtable_createCoun();

    for (int i = 11; i < argc-1; i++){
        hashtable_addCoun(countries, argv[i]);
    }
    
    
    //ΑΡΧΙΚΟΠΟΙΗΣΗ: Αναμονή για χώρες
    if ( store_records(citizens, &virus_list, countries) == -1 ){
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

    printf("Before listening\n");
    // Listening for connections
    if ( listen(sock, 2) < 0 ){
        perror("ERROR: during listening");
        exit(1);
    }
    printf("Listening for connections to port %d\n", port);








    //ΑΠΟΣΤΟΛΗ ΤΩΝ Bloomfilter (για κάθε ίωση)

    // Accepting a connection
    clientlen = sizeof(client);
    if ( (newsock = accept(sock, clientptr, &clientlen)) < 0 ){
        perror("ERROR: during accepting of connection");
        exit(1);
    }

    printf("Accepted Connection\n");
    
   
    /*if (send_bloomfilters(newsock, virus_list, socketBufferSize) == -1){
        perror("ERROR in sending bloomfilters");
        exit(1);
    }*/
        
    
    //close(newsock);

    //////////////////////////////////////////////////////////

    //ΑΠΟΣΤΟΛΗ ΜΗΝΥΜΑΤΟΣ ΕΤΟΙΜΟΤΗΤΑΣ ΓΙΑ ΑΙΤΗΜΑΤΑ

    // Accepting a connection
    /*if ( (newsock = accept(sock, clientptr, &clientlen)) < 0 ){
        perror("ERROR: during accepting of connection");
        exit(1);
    }

    printf("Accepted Connection\n");*/
    
    send_message(newsock, "READY", strlen("READY")+1, socketBufferSize);

    //close(newsock);






    //close(sock); 





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


    close(newsock);


    //////////////////////////////////////////////////

    //PRINT STRUCTURES
    /*printf("\n\nPRINT STRUCTURES: %s\n", argv[1]);
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries, stdout);*/
    

    //RELEASE OF MEMORY
    printf("\nRELEASE OF MEMORY.\n");
    hashtable_destroyCoun(countries);
    hashtable_destroy(citizens);
    deletelist(&virus_list);
    free(citizens);
    free(countries);


    printf("Monitor pid:%d exiting...\n", getpid());

    return 0;
}







int store_records(Hashtable citizens, struct List** virus_list, CountryHash countries){

    char* foldername;
    DIR *subdr;
    struct dirent *dfiles;

    char *filepath;



    struct Country* country_str;

    if (countries == NULL){
        printf("Hashtable is Empty!\n");
        return -1;
    }

    struct BucketCoun* current_buc;

    //SEARCH HASHTABLE OF COUNTRIES
    for (int i = 0; i < TABLE_SIZE; i++){

        current_buc = countries[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){ 

                if (current_buc->element[j].name != NULL){ //for every country

                    country_str = &(current_buc->element[j]);

                    //Find country's folder name
                    foldername = malloc( sizeof(char) * ( strlen("inputfolder") + strlen(country_str->name) + 2 ));

                    strcpy(foldername, "inputfolder");
                    strcat(foldername, "/");
                    strcat(foldername, country_str->name);


                    if ( (subdr = opendir(foldername)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   
                    country_str->num_of_files_read = 0;

                    //FOR EVERY FILE in the directory
                    while ( (dfiles = readdir(subdr)) != NULL ){

                        if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                            
                            //build path
                            filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(foldername) + 2 ) );         //+1 for the character '/'
                            strcpy(filepath, foldername);
                            strcat(filepath, "/");
                            strcat(filepath, dfiles->d_name);

                            //printf("file to open: %s\n", filepath);

                            if ( read_file(filepath, citizens, virus_list, country_str->name) == -1 ){
                                perror("ERROR: Unsuccessful Reading!");
                                return -1;
                            } /*else {
                                //printf("Successful reading of file %s!\n", dfiles->d_name);
                            }*/



                            free(filepath);

                            country_str->num_of_files_read++;
                            


                        }


                    }


                    closedir(subdr);
                    free(foldername);

                }
            }
            
            current_buc = current_buc->next_buc;  

        }
    
           
    } 


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

            }


            signal_num = 0;

        } else if ( signal_num == 2){      //GOT A SIGUSR1 SIGNAL

            
            //ΔΙΑΒΑΣΕ ΝΕΑ ΑΡΧΕΙΑ
            //Γέμισε τις δομές

            if ( read_new_files(citizens, viruslist, countries) == -1){
                printf("ERROR in Reading new Files\n");
            }

            printf("Finished with reading new Files!\n");

            //Αποστολή των Bloomfilter (για κάθε ίωση)
            if (send_bloomfilters(sock, *viruslist, socketBufferSize) == -1){
                perror("ERROR in sending bloomfilters");
                exit(1);
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
            
            //return;


        } 




    }



}




int read_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries){

    char* foldername;
    DIR *subdr;
    struct dirent *dfiles;

    int num_of_files = 0;
    int file_number;
    char *filepath;



    struct Country* country_str;

    if (countries == NULL){
        printf("Hashtable is Empty!\n");
        return -1;
    }

    struct BucketCoun* current_buc;

    //SEARCH HASHTABLE OF COUNTRIES
    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = countries[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){ 

                if (current_buc->element[j].name != NULL){ //for every country

                    country_str = &(current_buc->element[j]);

                    //Find country's folder name
                    foldername = malloc( sizeof(char) * ( strlen("inputfolder") + strlen(country_str->name) + 2 ));

                    strcpy(foldername, "inputfolder");
                    strcat(foldername, "/");
                    strcat(foldername, country_str->name);


                    if ( (subdr = opendir(foldername)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   
                    //get previous number of files in country
                    num_of_files = country_str->num_of_files_read;

                    //FOR EVERY FILE in the directory
                    while ( (dfiles = readdir(subdr)) != NULL ){

                        if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                            
                            file_number = atoi( strchr(dfiles->d_name, '-') + 1);


                            //if filenumber greater than initial number of files in country
                            // => it's a new file
                            if ( file_number >= country_str->num_of_files_read){

                                //THIS IS A NEW FILE


                                //build path
                                filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(foldername) + 2 ) );         //+1 for the character '/'
                                strcpy(filepath, foldername);
                                strcat(filepath, "/");
                                strcat(filepath, dfiles->d_name);

                                //printf("file to open: %s\n", filepath);

                                if ( read_file(filepath, citizens, virus_list, country_str->name) == 0 ){
                                    printf("Successful reading of file %s!\n", dfiles->d_name);
                                } else {
                                    perror("ERROR: Unsuccessful Reading!");
                                    return -1;
                                }



                                free(filepath);

                                num_of_files++;
                            }


                        }


                    }

                    country_str->num_of_files_read = num_of_files;

                    closedir(subdr);
                    free(foldername);

                }
            }
            
            current_buc = current_buc->next_buc;  

        }
    
           
    } 


    return 0;


}