//main.c

#include "console.h"
#include "records.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "citizens.h"
#include "countries.h"


int main( int argc, char *agrv[]){

    char filename[] = "citizenRecordsFile.txt"; //must be given in argv

    //List of viruses
    struct List* virus_list = NULL;

    //Hashtable of citizens
    Hashtable citizens;
    citizens = hashtable_create();

    //Hashtable of Countries
    CountryHash countries;
    countries = hashtable_createCoun();


    //List of countries???



    //GENERATE FILE






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
    citizens = NULL;
    virus_list = NULL;


    return 0;
}


