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


size_t bufferSize =10000;

extern int size_in_bytes;   //-s
extern int signal_num;


int store_records(int fd_r, char* pipename, Hashtable citizens, struct List** virus_list, CountryHash countries);
void monitor_body(int fd_w, Hashtable citizens, struct List **viruslist, CountryHash countries);
int read_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries);






int main(int argc, char* argv[]){

    printf("Hello from Monitor pid:%d with pipe %s and %s!\n", getpid(), argv[1], argv[2]);


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
    message = get_message_wrong( fd_r, sizeof(long));         //get bufferSize
    if ( strcmp(message, "error") == 0){
        exit(2);
    }


    bufferSize = atoi(message);
    printf("bufferSize: %ld\n", bufferSize);

    free(message);

    ///////////////////////////////////////????
    //GET BLOOMSIZE 
    message = get_message_wrong( fd_r,  sizeof(int));         //get bufferSize
    if ( strcmp(message, "error") == 0){
        exit(2);
    }

    size_in_bytes = atoi(message);
    printf("Bloomsize: %d\n", size_in_bytes);

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
    //διαβάζει μέσω των named pipes τις χώρες που θα αναλάβει
    if ( store_records(fd_r, argv[1], citizens, &virus_list, countries) == -1 ){
        perror("ERROR in storing records");
        exit(1);
    } 



    //Αποστολή των Bloomfilter (για κάθε ίωση)
    if (sent_bloomfilters(fd_w, virus_list, bufferSize) == -1){
        perror("ERROR in sending bloomfilters");
        exit(1);
    }



    //Αποστολή μηνύματος ετοιμότητας για αιτήματα
    sent_message_wrong(fd_w, "READY", bufferSize);

    ////////////////////////////////
    //SIGNAL HANDLING

    static struct sigaction sa1;        //SIGINT   CNTR+C
    static struct sigaction sa2;        //SIGQUIT    Ctrl + \ or Ctrl + Y
    static struct sigaction sa3;        //SIGUSR1      kill -s USR1 pid

    sa1.sa_handler = handle_MonitorFin;
    sigfillset (&(sa1.sa_mask));
    sigaction(SIGINT , &sa1 , NULL);

    sa2.sa_handler = handle_MonitorFin;
    sigfillset (&(sa2.sa_mask));
    sigaction(SIGQUIT , &sa2 , NULL);

    sa3.sa_handler = handle_newfiles;
    sigfillset (&(sa3.sa_mask));
    sigaction(SIGUSR1 , &sa3 , NULL);
    
    
    //////////////////////////////////
    //MAIN FUNCTION OF MONITORS
    monitor_body(fd_w, citizens, &virus_list, countries);





    //////////////////////////////////////////////////

    //PRINT STRUCTURES
    /*printf("\n\nPRINT STRUCTURES: %s\n", argv[1]);
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries);*/
    
    /*
    //SIMULATION
    printf("\n\nSIMULATION:\n");
    console(&virus_list, citizens, countries);
    */
   
    //PRINT STRUCTURES
    /*printf("\n\nPRINT STRUCTURES:\n");
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries);*/


    //RELEASE OF MEMORY
    printf("\nRELEASE OF MEMORY.\n");
    hashtable_destroyCoun(countries);
    hashtable_destroy(citizens);
    deletelist(&virus_list);
    free(citizens);
    free(countries);
    citizens = NULL;
    virus_list = NULL;
/*  free(input_dir);*/
    
    close(fd_r);
    close(fd_w);


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
        country_dir_name = get_message_wrong(fd_r, bufferSize);

        printf("Monitor with %s gets directory: %s\n", pipename, country_dir_name);

        //CHECK IF DONE
        if ( strcmp(country_dir_name, "DONE") == 0 ){
            printf("DONE\n");
            free(country_dir_name);
            break;
        }

        //GET NAME OF COUNTRY
        count_name = strchr( country_dir_name, '/' ); 

        if ( count_name != NULL ){
            count_name++;                   
            printf( "The folder is: %s\n", count_name ); // For example print it to see the result
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

                printf("        File: %s\n", dfiles->d_name);

                num_of_files++;


                char *filepath;
                filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(country_dir_name) + 2 ) );         //+1 for the character '/'
                strcpy(filepath, country_dir_name);
                strcat(filepath, "/");
                strcat(filepath, dfiles->d_name);

                //printf("file to open: %s\n", filepath);

                printf("READFILE: %s\n", filepath);
                if ( read_file(filepath, citizens, virus_list, count_name) == 0 ){
                    printf("Successful reading of file %s!\n", dfiles->d_name);
                } else {
                    perror("ERROR: Unsuccessful Reading!");
                    return -1;
                }
                

                free(filepath);

            }


        }
            
        set_num_of_files(countries, count_name, num_of_files);

        printf("NUM OF FILES FOR %s IS: %d\n", count_name, num_of_files);

        closedir(subdr);
        free(country_dir_name);
        //free(count_name);
        

    }


    return 0;


}






void monitor_body(int fd_w, Hashtable citizens, struct List **viruslist, CountryHash countries){


    int total_requests = 0;
    int accepted_req = 0;
    int rejected_req = 0;



    while( 1 ){


        if ( signal_num == 2){      //GOT A SIGUSR1 SIGNAL

            printf("IN BODY: SIGUSR1\n");
            
            //Λήψη SIGUSR
            //ΔΙΑΒΑΣΕ ΝΕΑ ΑΡΧΕΙΑ
            //Γέμισε τις δομές

            if ( read_new_files(citizens, viruslist, countries) == -1){
                printf("ERROR in Reading new Files\n");

            }

            //Αποστολή των Bloomfilter (για κάθε ίωση)
            /*if (sent_bloomfilters(fd_w, viruslist, bufferSize) == -1){
                perror("ERROR in sending bloomfilters");
                exit(1);
            }*/



            //Στείλε τα νέα bloomfilter στον parent

            
            signal_num = 0;

        } else if ( signal_num == 3 ){      //GOT A SIGINT/SIGQUIT SIGNAL

            printf("IN BODY: SIGINT/SIGQUIT\n");
            
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
            print_hashtableCoun(countries, logfile);

            fprintf(logfile, "%d\n%d\n%d", total_requests, accepted_req, rejected_req);
            
            signal_num = 0;
            return;


        } 




    }



}




int read_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries){

    //char *country_dir_name;
    //char *count_name;
    char* foldername;

    //READ FILES
    DIR *subdr;
    struct dirent *dfiles;

    int num_of_files = 0;
    int file_number;
    char *filepath;



    //FOR EVERY COUNTRY
    struct Country* country_str;

    if (countries == NULL){
        printf("Hashtable is Empty!\n");
        return -1;
    }

    struct BucketCoun* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = countries[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                if (current_buc->element[j].name != NULL){

                    country_str = &(current_buc->element[j]);

                    //Find country's folder name
                    foldername = malloc( sizeof(char) * ( strlen("inputfolder") + strlen(country_str->name) + 2 ));

                    strcpy(foldername, "inputfolder");
                    strcat(foldername, "/");
                    strcat(foldername, country_str->name);

                    printf("FOLDER NAME: %s\n", foldername);


                    if ( (subdr = opendir(foldername)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   

                    //FOR EVERY FILE in the directory
                    while ( (dfiles = readdir(subdr)) != NULL ){

                        if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                            printf("        File: %s\n", dfiles->d_name);

                            
                            file_number = atoi( strchr(dfiles->d_name, '-') + 1);
                            printf("FileNumber: %d\n", file_number);    //GET THE NUMBER OF THE FILE


                            //get previous number of files in country
                            num_of_files = country_str->num_of_files_read;



                            //if filenumber greater than initial number of files in country
                            // => it's a new file
                            if ( file_number > country_str->num_of_files_read){

                                //THIS IS A NEW FILE


                                //build path
                                filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(foldername) + 2 ) );         //+1 for the character '/'
                                strcpy(filepath, foldername);
                                strcat(filepath, "/");
                                strcat(filepath, dfiles->d_name);

                                //printf("file to open: %s\n", filepath);

                                printf("READFILE: %s\n", filepath);
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


                }
            }
            
            current_buc = current_buc->next_buc;  

        }
    
           
    } 


    return 0;


}