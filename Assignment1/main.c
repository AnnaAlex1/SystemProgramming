//main.c

#include "console.h"
#include "records.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "citizens.h"
#include "countries.h"


extern int size_in_bits;



int main( int argc, char *argv[]){

    if ( argc != 5 ){
        printf("Wrong number of arguments!\n");
        return -1;
    }

    if ( (strcmp(argv[1], "-c") != 0  && strcmp(argv[3], "-c") != 0)
        || (strcmp(argv[1], "-b") != 0  && strcmp(argv[3], "-b") != 0) 
        ){
                printf("Arguments: %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);

            printf("Wrong Input!\n");
            return -1;
    }



    printf("Arguments: %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);

    //char filename[] = "citizenRecordsFile.txt"; //must be given in argv

    char *filename;

    if ( strcmp(argv[1], "-c") == 0 ){

        filename = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(filename,argv[2]);
        size_in_bits = atoi(argv[4]);

    } else {
        
        filename = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(filename,argv[4]);
        size_in_bits = atoi(argv[2]);

    }


    //List of viruses
    struct List* virus_list = NULL;

    //Hashtable of citizens
    Hashtable citizens;
    citizens = hashtable_create();

    //Hashtable of Countries
    CountryHash countries;
    countries = hashtable_createCoun();




    //READ FILE 
    if ( read_file(filename, citizens, &virus_list, countries) == 0 ){
        printf("Successful reading of file!\n");
    }



    //PRINT STRUCTURES
    printf("\n\nPRINT STRUCTURES:\n");
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries);


    //SIMULATION
    printf("\n\nSIMULATION:\n");
    console(&virus_list, citizens, countries);

   


    //RELEASE OF MEMORY
    printf("\nRELEASE OF MEMORY.\n");
    hashtable_destroyCoun(countries);
    hashtable_destroy(citizens);
    deletelist(&virus_list);
    free(citizens);
    free(countries);
    citizens = NULL;
    virus_list = NULL;
    free(filename);


    return 0;
}


