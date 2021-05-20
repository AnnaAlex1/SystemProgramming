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
#include<sys/wait.h>

#include "citizens.h"
#include "virus.h"
#include "countries.h"
#include "records.h"
#include "sh_pipes.h"
#include "console.h"
#include "signal_handl.h"
#include "citizens.h"
#include "countries.h"


size_t bufferSize;

extern int size_in_bytes;
extern int signal_num;


int store_records(int fd_r, char* pipename, Hashtable citizens, struct List** virus_list, CountryHash countries);
void monitor_body(int fd_w, int fd_r, Hashtable citizens, struct List **viruslist, CountryHash countries);
int read_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries);






int main(int argc, char* argv[]){

    printf("Hello from Monitor pid:%d with pipes %s and %s!\n", getpid(), argv[1], argv[2]);


    //check for right number of arguments
    if (argc != 3){
        perror("Wrong number of arguments!");
    }


    // OPEN NAMED PIPE FOR READING
    int fd_r = open(argv[1], O_RDONLY);
    if ( fd_r < 0 ){
        perror("ERROR: in opening Named Pipe for Reading (from Monitor)");
        exit(1);
    }


    // OPEN NAMED PIPE FOR  WRITING
    int fd_w = open(argv[2], O_WRONLY);
    if ( fd_w < 0 ){
        perror("ERROR: in opening Named Pipe for Writing(from Monitor)");
        exit(1);
    }




    ////////////////////////////////////////////
    //GET bufferSize    
    char *message;
    message = get_message( fd_r, sizeof(long));         //get bufferSize
    if ( strcmp(message, "error") == 0){
        exit(2);
    }


    bufferSize = atoi(message);
    printf("Monitor Received BUFFERSIZE of %ld\n", bufferSize);

    free(message);

    ///////////////////////////////////////
    //GET BLOOMSIZE 
    message = get_message( fd_r,  bufferSize);         //get bloomSize
    if ( strcmp(message, "error") == 0){
        exit(2);
    }

    size_in_bytes = atoi(message);
    printf("Monitor Received BLOOMSIZE of %d\n", size_in_bytes);

    free(message);
    ////////////////////////////////////////////





    //List of viruses
    struct List* virus_list = NULL;

    //Hashtable of citizens
    Hashtable citizens;
    citizens = hashtable_create();

    //Hashtable of Countries
    CountryHash countries;
    countries = hashtable_createCoun();

    

    //ΑΡΧΙΚΟΠΟΙΗΣΗ: Αναμονή για χώρες
    if ( store_records(fd_r, argv[1], citizens, &virus_list, countries) == -1 ){
        printf("ERROR in storing records");
        exit(1);
    }


    
    //Αποστολή των Bloomfilter (για κάθε ίωση)
    if (send_bloomfilters(fd_w, virus_list, bufferSize) == -1){
        perror("ERROR in sending bloomfilters");
        exit(1);
    }


    
    //Αποστολή μηνύματος ετοιμότητας για αιτήματα
    send_message(fd_w, "READY", strlen("READY")+1, bufferSize);


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
    monitor_body(fd_w, fd_r, citizens, &virus_list, countries);





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

    
    close(fd_r);
    close(fd_w);

    printf("Monitor pid:%d exiting...\n", getpid());

    return 0;
}







int store_records(int fd_r, char* pipename, Hashtable citizens, struct List** virus_list, CountryHash countries){

    char *country_dir_name;
    char *count_name;

    //READ FILES
    DIR *subdr;
    struct dirent *dfiles;

    int num_of_files = 0;

    while(1) {

        //GET DIRECTORY
        country_dir_name = get_message(fd_r, bufferSize);

        //printf("Monitor with %s gets directory: %s\n", pipename, country_dir_name);

        //CHECK IF DONE
        if ( strcmp(country_dir_name, "DONE") == 0 ){
            free(country_dir_name);
            break;
        }

        //GET NAME OF COUNTRY
        count_name = strchr( country_dir_name, '/' ); 

        if ( count_name != NULL ){
            count_name++;                   
        } else {
            printf("Wrong Folder Name\n");
            return -1;
        }



        hashtable_addCoun(countries, count_name);   //add country to hashtable
        
        
        
        num_of_files = 0;   //keep count of files for this country

        if ( (subdr = opendir(country_dir_name)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   
        //FOR EVERY FILE in the directory
        while ( (dfiles = readdir(subdr)) != NULL ){

            if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                //printf("        File: %s\n", dfiles->d_name);

                num_of_files++;


                char *filepath;
                filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(country_dir_name) + 2 ) );         //+1 for the character '/'
                strcpy(filepath, country_dir_name);
                strcat(filepath, "/");
                strcat(filepath, dfiles->d_name);

                //read file
                if ( read_file(filepath, citizens, virus_list, count_name) != 0 ){
                    perror("ERROR: Unsuccessful Reading!");
                    return -1;
                }
                

                free(filepath);

            }


        }
        
        //number of files for this country
        set_num_of_files(countries, count_name, num_of_files);

        closedir(subdr);
        free(country_dir_name);
        

    }


    return 0;


}






void monitor_body(int fd_w, int fd_r, Hashtable citizens, struct List **viruslist, CountryHash countries){


    int total_requests = 0;
    int accepted_req = 0;
    int rejected_req = 0;

    char *command;

    printf("\nMonitor %d Waiting For Instructions\n", getpid());

    while( 1 ){


        if ( signal_num == 5){      // SIGNAL FOR READING

            //GET CHOSEN COMMAND
            command = get_message(fd_r, bufferSize);

            if ( strcmp(command, "travelRequest") == 0 ){

                total_requests++;

                //GET NECESSARY INFO
                //get name of virus
                char *virus = get_message(fd_r, bufferSize);
                //get citizenID
                char *citizenID = get_message(fd_r, bufferSize);


                //get struct for virus
                struct List* virusNode = getelemfromlist(*viruslist, virus);
                if ( virusNode == NULL ){
                    printf("No such Virus!\n");
                    send_message(fd_w, "Error", strlen("Error")+1, bufferSize);
                    continue;
                }

                //try to get a date of vaccination
                char *date;
                if ( (date = getDate_VacSkipList(virusNode->vaccinated, citizenID) ) != NULL ){
                    //found
                    printf("VACCINATED ON %s\n", date);
                    
                    //send answer "YES"
                    send_message(fd_w, "YES", strlen("YES")+1, bufferSize);

                    //send date
                    send_message(fd_w, date, strlen(date)+1, bufferSize);

                    accepted_req++;

                } else {    //no record for this person
                    printf("NOT VACCINATED\n");

                    //send answer "NO"
                    send_message(fd_w, "NO", strlen("NO")+1, bufferSize);

                    rejected_req++;
                }



            } else if ( strcmp(command, "searchVaccinationStatus") == 0 ){

                char *citizenID = get_message(fd_r, bufferSize);

                struct Citizen *cit = hashtable_get(citizens, citizenID);

                if ( cit == NULL ){     //citizen not in this monitor
                    
                    send_message(fd_w, "NOT FOUND", strlen("NOT FOUND")+1, bufferSize);

                } else {        //citizen found

                    send_message(fd_w, "FOUND", strlen("FOUND")+1, bufferSize);

                    //send info
                    char info[300];
                    sprintf(info, "%s %s %s %s", cit->citizenID, cit->firstname, cit->lastname, cit->country);

                    //send citizen details
                    send_message(fd_w, info, strlen(info)+1, bufferSize);

                    //send age
                    sprintf(info, "%d", cit->age);
                    send_message(fd_w, info, strlen(info)+1, bufferSize );


                    struct List *temp = *viruslist;
                    char *vac_date;
                    

                    while ( temp != NULL){  //for every virus

                        vac_date = getDate_VacSkipList( temp->vaccinated, citizenID );

                        if ( vac_date == NULL ){    //not yet vaccinated
                            
                            sprintf(info,"%s %s", temp->name, " NOT YET VACCINATED");

                        } else {        //vaccinated

                            sprintf(info,"%s %s %s", temp->name, " VACCINATED ON ", vac_date);

                        }

                        send_message(fd_w, info, strlen(info)+1, bufferSize);
                        

                        temp = temp->next;
                    }

                    //send "DONE" message
                    send_message(fd_w, "DONE", strlen("DONE")+1, bufferSize);

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
            if (send_bloomfilters(fd_w, *viruslist, bufferSize) == -1){
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