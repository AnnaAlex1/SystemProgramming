//records.c

#include "records.h"
#include "citizens.h"
#include "countries.h"
#include "virus.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



int read_file(char* filename, Hashtable ht, struct List** virus_list, CountryHash countries){

    FILE* cit_file;

    cit_file = fopen(filename, "r");
    if ( cit_file == NULL){
        perror("Cannot open file!\n");
        return -1;
    }


    char record[LINE_LEN];
    struct Citizen citizen;
    struct Citizen *fromHash;
    struct Country *fromhash_coun;

    char* virus;
    bool done;
    char* date;

    struct List* virlist_temp;

    struct VacSkipRecord* vac_element = NULL;
    struct NotVacSkipRecord* not_vac_elem = NULL;


    while( fgets(record, LINE_LEN, cit_file) != NULL){


        citizen.citizenID = strtok(record, " ");
        citizen.firstname = strtok(NULL, " ");
        citizen.lastname = strtok(NULL, " ");
        citizen.country = strtok(NULL, " ");
        citizen.age = atoi(strtok(NULL, " "));
        virus = strtok(NULL, " ");
        if ( strcmp(strtok(NULL, " "), "YES") == 0){
            done = true;
        } else {
            done = false;
        }

        date = strtok(NULL, "\n");

        
        printf("id: %s\n", citizen.citizenID);
        printf("name: %s\n", citizen.firstname);
        printf("surname: %s\n", citizen.lastname);
        printf("country: %s\n", citizen.country);
        printf("age: %d\n", citizen.age);
        printf("virus: %s\n", virus);
        printf("done: %d\n", done);
        printf("date: %s\n", date);
        
        


        //checking for this id in the Citizens hashtable
        fromHash = hashtable_get(ht, citizen.citizenID);
        if ( fromHash != NULL ){    //citizen already in hashtable

            if ( strcmp(fromHash->firstname, citizen.firstname) != 0 
                || strcmp(fromHash->lastname, citizen.lastname) != 0 
                || strcmp(fromHash->country, citizen.country) != 0 
                || fromHash->age != citizen.age
                || !check_done(done, citizen.citizenID, virus, *virus_list)
                || !check_date(done, citizen.citizenID, virus, date, *virus_list ) 
                ){

                    printf("ERROR IN RECORD:   %s\n", citizen.citizenID);
                    
                    continue;

            }

            
        } else {

            //put citizen in hashtable
            hashtable_add(ht, &citizen);
            
            //put country in hashtable
            if ( ( fromhash_coun = hashtable_getCoun(countries, citizen.country)) == NULL ){   //if it doesn't exist
                hashtable_addCoun(countries, citizen.country);
            } else {    //increase 
                fromhash_coun->population++;
            }     
        }  


        //put virus in list
        if ( checkifinlist(*virus_list, virus) == -1 ){
            addinlist(virus_list, virus);
        }

        virlist_temp = *virus_list;
        while ( virlist_temp != NULL){
            
            if ( strcmp(virlist_temp->name, virus) == 0){

                if ( done ){
                    insert_Bloom( *(virlist_temp->vacc_bloom), NUM_OF_HASHES, citizen.citizenID);
                    vac_element = malloc(sizeof(struct VacSkipRecord));
                    vac_element->name = malloc(sizeof(char) * (strlen(citizen.citizenID)+1));
                    vac_element->date = malloc(sizeof(char) * (strlen(date)+1));
                    strcpy(vac_element->name, citizen.citizenID);
                    strcpy(vac_element->date, date);
                    addVacSkipList( &(virlist_temp->vaccinated), vac_element);
                } else {
                    insert_Bloom( *(virlist_temp->not_vacc_bloom), NUM_OF_HASHES, citizen.citizenID);
                    not_vac_elem = malloc(sizeof(struct NotVacSkipRecord));
                    not_vac_elem->name = malloc(sizeof(char) * (strlen(citizen.citizenID)+1));
                    strcpy(not_vac_elem->name, citizen.citizenID);
                    addNotVacSkipList( &(virlist_temp->not_vacc), not_vac_elem);
                }



            }
            virlist_temp = virlist_temp->next;
        }

        
        printf("\n\n");


    }
    
    fclose(cit_file);

    return 0;
}






bool check_done(bool done, char* id, char* virus, struct List *virus_list){


    struct List* virus_temp = virus_list;

    while( virus_temp != NULL){

        if ( strcmp(virus_temp->name, virus ) == 0){

            //if there is a record in vaccinated list of this virus
            if ( !done && search_Bloom(*(virus_temp->vacc_bloom), NUM_OF_HASHES, id) ){
                //search in list
                if ( searchVacSkipList(virus_temp->vaccinated, id) ){
                    return false;
                }
            }

            //if there is a record in not vaccinated list of this virus
            if ( done && search_Bloom(*(virus_temp->not_vacc_bloom), NUM_OF_HASHES, id)){
                //search in list
                if ( searchNotVacSkipList(virus_temp->not_vacc, id) ){
                    return false;
                }
            } 



            break;
        }

        virus_temp = virus_temp->next;
    }

    return true;
}






bool check_date(bool done, char* id, char* virus, char* date, struct List *virus_list){


    //if date is given but doesn't follow a "YES"
    if (date != NULL && done == false){
        return false;
    }


    //if there is a date and its different than the one in previous record
    if ( date != NULL){

        struct List* virus_temp = virus_list;
        char *ret_date;

        while( virus_temp != NULL){

            if ( strcmp(virus_temp->name, virus ) == 0){

                //if there is a record in vaccinated list of this virus
                if ( search_Bloom( *(virus_temp->vacc_bloom), NUM_OF_HASHES, id) ){
                    //search in list
                    
                    if ( ( ret_date = getDate_VacSkipList(virus_temp->vaccinated, id)) != NULL ){
                        if ( strcmp(ret_date, date)  != 0 ){
                            return false;
                        }
                        
                    }
                }

                break;
            }

            virus_temp = virus_temp->next;
        }


    }



    return true;

}



