//records.c

#include "records.h"
#include "citizens.h"
#include "countries.h"
#include "virus.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



int read_file(char* filename, Hashtable ht, struct List** virus_list, CountryHash countries, char *country){

    FILE* cit_file;

    //open file with records
    cit_file = fopen(filename, "r");
    if ( cit_file == NULL){
        perror("Cannot open file!\n");
        return -1;
    }


    char record[LINE_LEN];
    struct Citizen citizen;


    char* virus;
    bool done;
    char* date;



    while( fgets(record, LINE_LEN, cit_file) != NULL){  //for every line


        citizen.citizenID = strtok(record, " ");
        citizen.firstname = strtok(NULL, " ");
        citizen.lastname = strtok(NULL, " ");
        citizen.country = malloc(sizeof(char)*(strlen(country)+1));
        strcpy(citizen.country, country);
        char *age_str = strtok(NULL, " ");
        virus = strtok(NULL, " ");
        char *done_str = strtok(NULL, "\n");    //handling case date is missing
        done_str = strtok(done_str, " ");
        date = strtok(NULL, "\n");


        if ( citizen.citizenID == NULL || citizen.firstname == NULL
            || citizen.lastname == NULL || citizen.country == NULL
            || age_str == NULL || virus == NULL || done_str == NULL ){
                printf("ERROR IN RECORD\n");
                continue;

            }


        citizen.age = atoi(age_str);
        if ( strcmp(done_str, "YES") == 0){
            done = true;
        } else {
            done = false;
        }


        /*
        printf("id: %s\n", citizen.citizenID);
        printf("name: %s\n", citizen.firstname);
        printf("surname: %s\n", citizen.lastname);
        printf("country: %s\n", citizen.country);
        printf("age: %d\n", citizen.age);
        printf("virus: %s\n", virus);
        printf("done: %d\n", done);
        printf("date: %s\n", date);
        */        

        insert_record(ht, virus_list, countries ,citizen, virus, done, date, false);
        
        free(citizen.country);

    }
    
    fclose(cit_file);

    return 0;
}






void insert_record(Hashtable ht,  struct List** virus_list, CountryHash countries, struct Citizen citizen, char* virus, bool done, char* date, bool from_console){
    struct Citizen *fromHash;

    struct List* virlist_temp;

    struct VacSkipRecord* vac_element = NULL;
    struct NotVacSkipRecord* not_vac_elem = NULL;

    //if date is given but doesn't follow a "YES"
    if (date != NULL && done == false){
        printf("ERROR IN RECORD:   %s\n", citizen.citizenID);
        return;
    }

    
    //if date is not given and follows a "YES"
    if (date == NULL && done == true){
        printf("ERROR IN RECORD:   %s\n", citizen.citizenID);
        return;
    }
        


    //checking for this id in the Citizens hashtable
    fromHash = hashtable_get(ht, citizen.citizenID);
    if ( fromHash != NULL ){    //citizen already in hashtable


        //check if infos match
        if ( strcmp(fromHash->firstname, citizen.firstname) != 0 
            || strcmp(fromHash->lastname, citizen.lastname) != 0 
            || strcmp(fromHash->country, citizen.country) != 0 
            || fromHash->age != citizen.age
            || !check_done(done, citizen.citizenID, virus, *virus_list, from_console)
            || !check_date(done, citizen.citizenID, virus, date, *virus_list ) 
            ){

                printf("ERROR IN RECORD:   %s\n", citizen.citizenID);
                
                return;

        }
        
    } else {

        //put citizen in hashtable
        hashtable_add(ht, citizen);

            
    }  


    //put virus in list
    if ( checkifinlist(*virus_list, virus) == -1 ){
        addinlist(virus_list, virus);
    }

    virlist_temp = *virus_list;

    while ( virlist_temp != NULL){
 
        if ( strcmp(virlist_temp->name, virus) == 0){

            if ( done ){    //"YES"

                insert_Bloom( *(virlist_temp->vacc_bloom), NUM_OF_HASHES, citizen.citizenID);
                vac_element = malloc(sizeof(struct VacSkipRecord));
                vac_element->name = malloc(sizeof(char) * (strlen(citizen.citizenID)+1));
                vac_element->date = malloc(sizeof(char) * (strlen(date)+1));
                strcpy(vac_element->name, citizen.citizenID);
                strcpy(vac_element->date, date);
                addVacSkipList( &(virlist_temp->vaccinated), vac_element, &(virlist_temp->levels_vac));

            } else {  //  "NO"

                insert_Bloom( *(virlist_temp->not_vacc_bloom), NUM_OF_HASHES, citizen.citizenID);
                not_vac_elem = malloc(sizeof(struct NotVacSkipRecord));
                not_vac_elem->name = malloc(sizeof(char) * (strlen(citizen.citizenID)+1));
                strcpy(not_vac_elem->name, citizen.citizenID);
                addNotVacSkipList( &(virlist_temp->not_vacc), not_vac_elem,  &(virlist_temp->levels_notvac));
            }


        }
        virlist_temp = virlist_temp->next;
    }

    if ( from_console ){
        printf("SUCCESSFUL INSERTION\n");
    }


}











bool check_done(bool done, char* id, char* virus, struct List *virus_list, bool from_console){


    struct List* virus_temp = virus_list;

    while( virus_temp != NULL){

        if ( strcmp(virus_temp->name, virus ) == 0){


            //"NO" && there is a record in vaccinated list of this virus
            if ( !done && search_Bloom(*(virus_temp->vacc_bloom), NUM_OF_HASHES, id) ){
                //search in list
                if ( searchVacSkipList(virus_temp->vaccinated, id) ){
                    return false;
                }
            }



            // "YES" && there is a record in not vaccinated list of this virus
            if ( done && search_Bloom(*(virus_temp->not_vacc_bloom), NUM_OF_HASHES, id)){
                //search in list
                if ( searchNotVacSkipList(virus_temp->not_vacc, id) ){
                    
                    //citizen is registered as not vaccinated
                    if ( from_console ){    //inserting record from console
                        removeNotVacSkipList(&(virus_temp->not_vacc), id, &(virus_temp->levels_notvac));
                        return true;

                    } else {
                        printf("ERROR IN RECORD:   %s\n", id);
                        return false;

                    }

                }
            } 


            //"NO" && there is already a record in not vaccinated list of this virus
            if ( !done && search_Bloom( *(virus_temp->not_vacc_bloom), NUM_OF_HASHES, id) == 1 
                && searchNotVacSkipList( virus_temp->not_vacc, id) == 1){

                //citizen is registered as not vaccinated
                printf("ALREADY IN RECORD:   %s\n", id);
                return false;

            }


            char* ret_date;
            
            //"YES" && citizen is in vaccinated list of virus
            if ( done && search_Bloom( *(virus_temp->vacc_bloom), NUM_OF_HASHES, id) == 1 
                && (ret_date = getDate_VacSkipList( virus_temp->vaccinated, id)  ) != NULL
                ){
                    //citizen is registered as vaccinated
                    if ( from_console ){
                        printf("ERROR: CITIZEN ALREADY VACCINATED ON %s\n", ret_date);
                    }

                return false;  

            }

            break;
        }

        virus_temp = virus_temp->next;
    }

    return true;
}






bool check_date(bool done, char* id, char* virus, char* date, struct List *virus_list){


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




bool is_date(char* string){

    if ( string == NULL ){
        return 0;
    }

    char *temp = malloc(sizeof(char)*(strlen(string)+1));
    strcpy(temp, string);

    char* day = strtok(temp, "-");
    char* month = strtok(NULL, "-");
    char* year = strtok(NULL, "\0");

    if ( day == NULL || month == NULL || year == NULL){
        return 0;
    }


    int day_num = atoi(day);
    int month_num = atoi(month);
    int year_num = atoi(year);

    /*
    printf("day: %d\n", day_num);
    printf("month: %d\n", month_num);
    printf("year: %d\n", year_num);
    */

    if ( day_num>=1 && day_num <= 31 
        && month_num >=1 && month_num <=12 
        && year_num >= 1900 && year_num <= 2100){
        free(temp); 
        return 1;
    }

    free(temp);
    return 0;

}


