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


#include "citizens.h"
#include "virus.h"
#include "countries.h"
#include "records.h"
#include "sh_pipes.h"
#include "console.h"

#include<sys/wait.h>


#include "citizens.h"
#include "countries.h"

size_t buffersize =10000;

extern int size_in_bytes;   //-s


int store_records(int fd_r, char* pipename, Hashtable citizens, struct List** virus_list, CountryHash countries);

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
    //GET BUFFERSIZE    
    char *message;
    message = get_message_wrong( fd_r, sizeof(long));         //get buffersize
    if ( strcmp(message, "error") == 0){
        exit(2);
    }


    buffersize = atoi(message);
    printf("Buffersize: %ld\n", buffersize);

    free(message);

    ///////////////////////////////////////????
    //GET BLOOMSIZE 
    message = get_message_wrong( fd_r,  sizeof(int));         //get buffersize
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
    if (sent_bloomfilters(fd_w, virus_list, buffersize) == -1){
        perror("ERROR in sending bloomfilters");
        exit(1);
    }



    //Αποστολή μηνύματος ετοιμότητας για αιτήματα



    int total_requests = 0;
    int accepted_req = 0;
    int rejected_req = 0;

    //Λήψη SIGINT/SIGQUIT
        //Άνοιγμα αρχείου logfilexxx
        //εκτύπωση χωρών
        //εκτύπωση total_request
        //εκτύπωση accepted
        //εκτύπωση rejected

    //Λήψη SIGUSR
        //ΔΙΑΒΑΣΕ ΝΕΑ ΑΡΧΕΙΑ
        //Γέμισε τις δομές
        //Στείλε τα νέα bloomfilter στον parent








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



    while(1) {

        //GET DIRECTORY
        country_dir_name = get_message_wrong(fd_r, buffersize);

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
        }

        if ( (subdr = opendir(country_dir_name)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   
        //FOR EVERY FILE in the directory
        while ( (dfiles = readdir(subdr)) != NULL ){

            if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                printf("        File: %s\n", dfiles->d_name);

                char *filepath;
                filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(country_dir_name) + 2 ) );         //+1 for the character '/'
                strcpy(filepath, country_dir_name);
                strcat(filepath, "/");
                strcat(filepath, dfiles->d_name);

                //printf("file to open: %s\n", filepath);

                printf("READFILE: %s\n", filepath);
                if ( read_file(filepath, citizens, virus_list, countries, count_name) == 0 ){
                    printf("Successful reading of file %s!\n", dfiles->d_name);
                } else {
                    perror("ERROR: Unsuccessful Reading!");
                    return -1;
                }
                

                free(filepath);

            }


        }
            


        closedir(subdr);
        free(country_dir_name);
        

    }


    return 0;


}
