//console.c


#include "console.h"
#include "virus.h"
#include "citizens.h"
#include "records.h"
#include "countries.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> 


void console(struct List** VirusList, Hashtable citizens, CountryHash countries){

    char input[80];
    char *arg;

    struct List* virusNode = NULL;
    char *virusName;
    
    while(1){

        printf("Please give instructions\n");

        fgets(input,80, stdin);

        printf("input: %s\n",input);
        if ( strcmp( strtok(input, "\n"), "/exit") == 0){
            return;
        }

        arg = strtok(input, " ");
        //printf("Input: %s Arg: %s", input, arg);

        if ( strcmp(arg, "/vaccineStatusBloom") == 0){
            
            char *citizenID = strtok(NULL, " ");
            virusName = strtok(NULL, "\n");

            printf("CitizenID: %s\n", citizenID);
            printf("virusName: %s\n", virusName);

            virusNode = getelemfromlist(*VirusList, virusName);

            if ( search_Bloom( *(virusNode->vacc_bloom), NUM_OF_HASHES, citizenID) ){
                printf("MAYBE\n");
            } else {
                printf("NOT VACCINATED\n");
            }


        } else if ( strcmp(arg, "/vaccineStatus") == 0){            ///????????????

            char *citizenID = strtok(NULL, " ");
            virusName = strtok(NULL, "\n");

            printf("CitizenID: %s\n", citizenID);
            if ( !hashtable_find(citizens, citizenID) ){
                printf("No such Citizen\n");
                continue;
            }
            printf("virusName: %s\n", virusName);

            if ( virusName != NULL){ //a virus is given

                //get virus struct
                virusNode = getelemfromlist(*VirusList, virusName);

                char *date;
                if ( (date = getDate_VacSkipList(virusNode->vaccinated, citizenID) ) != NULL ){
                    //found
                    printf("VACCINATED ON %s\n", date);

                } else {    //no record for this person
                    printf("NOT VACCINATED\n");

                }

            } else {    //virus is not given                       ///????????????


                struct List* virus_temp = *VirusList;

                while (virus_temp != NULL){
                    
                    char* date;
                    //check vaccinated list for this virus
                    if ( (date = getDate_VacSkipList(virus_temp->vaccinated, citizenID) ) != NULL ){
                        printf("%s  YES     %s\n", virus_temp->name, date);
                    } //check not vaccinated list for this virus
                    else if ( searchNotVacSkipList(virus_temp->not_vacc, citizenID) == 1 ){
                        printf("%s  NO\n", virus_temp->name);
                    }

                    virus_temp = virus_temp->next;
                }

            }



        } else if ( strcmp(arg, "/populationStatus") == 0){

            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            char *arg3 = strtok(NULL, " ");
            char *arg4 = strtok(NULL, "\n");

            if ( arg4 == NULL ){ //case: country was not given

                printf("VirusName: %s,   Date1: %s,   Date2: %s\n", arg1, arg2, arg3);

                printf("NOT IMPLEMENTED\n");


            } else {    //country was given
                printf("Country: %s,  VirusName: %s,   Date1: %s,   Date2: %s\n", arg1, arg2, arg3, arg4);
            
                //get virus struct
                virusNode = getelemfromlist(*VirusList, arg2);

                if ( virusNode == NULL ) {printf("Error: no such virus\n"); continue; }

                VacSkipList vac_temp;
                char* citID_temp;
                struct Citizen *cit_temp;
                int num_of_vacc = 0;

                vac_temp = virusNode->vaccinated;

                while ( vac_temp->down != NULL){    //reach bottom
                    vac_temp = vac_temp->down;
                }

                vac_temp = vac_temp->next;
                while ( vac_temp != NULL){

                    citID_temp = vac_temp->elem->name;  //get citizen id
                    cit_temp = hashtable_get(citizens, citID_temp); //get citizen

                    //if citizen is from 'country' (arg1)
                    if ( strcmp(cit_temp->country, arg1) == 0 ){
                        if ( datecmp(arg3, vac_temp->elem->date) >= 0 && datecmp( vac_temp->elem->date, arg4 ) >= 0){
                            num_of_vacc++;
                        }
                    }
                    vac_temp = vac_temp->next;
                }

                struct Country* countryy = hashtable_getCoun(countries, arg1);

                //print results
                printf("%s %d %f%c\n", arg1, num_of_vacc, (float)num_of_vacc*100.0/(float)(countryy->population), '%' );

            
            }

            
        } else if ( strcmp(arg, "/popStatusByAge") == 0){
            
        } else if ( strcmp(arg, "/insertCitizenRecord") == 0){
            
            //get arguments
            struct Citizen cit;

            cit.citizenID = strtok(NULL, " ");
            cit.firstname = strtok(NULL, " ");
            cit.lastname = strtok(NULL, " ");
            cit.country = strtok(NULL, " ");
            cit.age = atoi(strtok(NULL, " "));
            
            virusName = strtok(NULL, " ");
            
            bool done;
            if ( strcmp(strtok(NULL, " "), "YES") == 0){
                done = true;
            } else {
                done = false;
            }
            char* date = strtok(NULL, "\n");

            insert_record(citizens, VirusList, countries, cit, virusName, done, date, true);


        } else if ( strcmp(arg, "/vaccinateNow") == 0){
            
            //get arguments
            char *citizenID = strtok(NULL, " ");
            char *firstname = strtok(NULL, " ");
            char *lastname = strtok(NULL, " ");
            char *country = strtok(NULL, " ");
            int age = atoi(strtok(NULL, " "));
            virusName = strtok(NULL, "\n");

            //if not all arguments were given
            if ( citizenID == NULL || firstname == NULL || lastname == NULL 
                || country == NULL || virusName == NULL){
                    printf("Please give all info!\n");
                    continue;
                }


            //check if info matches citizen in record
            struct Citizen* from_hash = hashtable_get(citizens, citizenID);

            if ( from_hash == NULL){    //check existence
                printf("No such citizen in record!\n");
                continue;
            } else {    //check if infos match
                if ( strcmp(from_hash->firstname, firstname) != 0 
                    || strcmp(from_hash->lastname, lastname) != 0 
                    || strcmp(from_hash->country, country) != 0 
                    || from_hash->age != age
                ){

                    printf("Error in given info!\n"); 
                    continue;

                }
            }

            //check if virus exists
            if ( checkifinlist(*VirusList, virusName ) == -1){
                printf("No such virus in record!\n");
                continue;
            } else {
                //get virus struct
                virusNode = getelemfromlist(*VirusList, virusName);
            }

            char* ret_date;

            //check if vaccinated
            if ( search_Bloom(*(virusNode->vacc_bloom), NUM_OF_HASHES, citizenID) == 1 ){

                    //search in list
                    if ( ( ret_date = getDate_VacSkipList( virusNode->vaccinated, citizenID)) != NULL ){
                        
                        //vaccination found
                        printf("ERROR: CITIZEN %s ALREADY VACCINATED ON %s\n", citizenID, ret_date);
                        continue;
                    }

            }
            
            struct VacSkipRecord* vac_element;
            char *cur_date = get_cur_date();

            /*
            printf("\nNotVaccinatedSkipList: BEFORE\n");    /////////////----------------
            printNotVacSkipList(virusNode->not_vacc);

            printf("\nVaccinatedSkipList: BEFORE\n");   /////////////--------------
            printVacSkipList(virusNode->vaccinated);
            */

            //vaccination not found - add new record
            insert_Bloom( *(virusNode->vacc_bloom), NUM_OF_HASHES, citizenID);
            vac_element = malloc(sizeof(struct VacSkipRecord));
            vac_element->name = malloc(sizeof(char) * (strlen(citizenID)+1));
            vac_element->date = malloc(sizeof(char) * (strlen(cur_date)+1));
            strcpy(vac_element->name, citizenID);
            strcpy(vac_element->date, cur_date);
            addVacSkipList( &(virusNode->vaccinated), vac_element);
            
            free(cur_date);

            //check if in not_vaccinated list
            if ( search_Bloom(*(virusNode->not_vacc_bloom), NUM_OF_HASHES, citizenID) == 1 ){

                    //search in list
                    if ( searchNotVacSkipList( virusNode->not_vacc, citizenID) == 1 ){
                        
                        //record found - remove
                        removeNotVacSkipList(&(virusNode->not_vacc), citizenID);
                    }

            }

            printf("\nVaccinatedSkipList: AFTER\n");    /////////////----------------
            printVacSkipList(virusNode->vaccinated);

            printf("\nNotVaccinatedSkipList: AFTER\n");    /////////////----------------
            printNotVacSkipList(virusNode->not_vacc);



        } else if ( strcmp(arg, "/list-nonVaccinated-Persons") == 0){

            virusName = strtok(NULL, "\n");

            NotVacSkipList skipl;
            struct Citizen* cit_temp;
            bool found = false;

            if ( virusName != NULL){

                virusNode = getelemfromlist(*VirusList, virusName);

                if ( virusNode != NULL){
                    
                    skipl = virusNode->not_vacc;

                    while (skipl->down != NULL){
                        //getting to lower point of list
                        skipl = skipl->down;
                    }

                    skipl = skipl->next;
                    while (skipl != NULL){
                        
                        cit_temp = hashtable_get(citizens, skipl->elem->name);

                        printf("%s %s %s %s %d\n", cit_temp->citizenID, cit_temp->firstname, cit_temp->lastname, cit_temp->country, cit_temp->age);
                        found = true;
                        skipl = skipl->next;
                    }


                    if (!found) printf("Empty list!\n");
                    
                } else {
                    printf("Error: no such virus!\n");
                }


            } else {
                printf("Error: Give virus\n");
            }
        }



    }   //end of while


    
}


int datecmp(char* date1, char* date2){

    char newdate1[12];
    strcpy(newdate1, date1);
    char newdate2[12];
    strcpy(newdate2, date2);



    char *day1 = strtok(newdate1, "-");
    char *month1 = strtok(NULL, "-");
    char *year1 = strtok(NULL, "\0");

    char *day2 = strtok(newdate2, "-");
    char *month2 = strtok(NULL, "-");
    char *year2 = strtok(NULL, "\0");

    int str_res;


    printf("Date1: %s %s %s\n", day1, month1, year1);
    printf("Date2: %s %s %s\n", day2, month2, year2);

    str_res = strcmp(year1, year2);
    if ( str_res < 0 ){   //if year2 > year1
        return 1;
    } else if ( str_res > 0 ){   //year1 > year2
        return -1;
    } else {    //case: year1 == year2

        str_res = strcmp(month1, month2);
        if ( str_res < 0 ){   //if month2 > month1
            return 1;
        } else if ( str_res > 0 ){   // month1 > month2
            return -1;
        } else {    //case: month1 == month2
            
            str_res = strcmp(day1, day2);
            if ( str_res < 0 ){   //if day2 > day1
                return 1;
            } else if ( str_res > 0 ){   // day1 > day2
                return -1;
            } else {    //case: day1 == day2
                return 0;
            }

        }

    }

    return 0;

}


char *get_cur_date(){

    time_t cur_time;

    time(&cur_time);


    struct tm *arith = localtime(&cur_time);
    char *cur_date = malloc( sizeof(char) * 11); //format: dd-mm-yyyy +\0
 
    char buffer[40];
    
    snprintf(buffer, 40, "%d-%d-%d", arith->tm_mday, arith->tm_mon+1, arith->tm_year + 1900);
    strcpy( cur_date, buffer);

    printf("DATEEEE: %s\n", cur_date);

    return cur_date;
}