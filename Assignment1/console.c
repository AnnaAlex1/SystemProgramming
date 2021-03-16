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

                if ( !is_date(arg2) || !is_date(arg3) ){
                    printf("Wrong Dates!\n");
                    continue;
                }
                
                populationStatus_all(*VirusList, citizens, countries, arg1, arg2, arg3);


            } else {    //country was given

                if ( !is_date(arg3) || !is_date(arg4) ){
                    printf("Wrong Dates!\n");
                    continue;
                }

                populationStatus_one(*VirusList, citizens, countries, arg1, arg2, arg3, arg4);
 
            }

            
        } else if ( strcmp(arg, "/popStatusByAge") == 0){

            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            char *arg3 = strtok(NULL, " ");
            char *arg4 = strtok(NULL, "\n");

            if ( arg1 == NULL || arg2 == NULL || arg3 == NULL){
                printf("Incorrect Input!\n");
                continue;
            }

            if ( arg4 == NULL ){ //case: country was not given

                printf("VirusName: %s,   Date1: %s,   Date2: %s\n", arg1, arg2, arg3);

                if ( !is_date(arg2) || !is_date(arg3) ){
                    printf("Wrong Dates!\n");
                    continue;
                }

                popStatusByAge_all(*VirusList, citizens, countries, arg1, arg2, arg3);



            } else {    //country was given

                if ( !is_date(arg3) || !is_date(arg4) ){
                    printf("Wrong Dates!\n");
                    continue;
                }    

                popStatusByAge_one(*VirusList, citizens, arg1, arg2, arg3, arg4);

            }

                





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
            addVacSkipList( &(virusNode->vaccinated), vac_element, &(virusNode->levels_vac));
            
            free(cur_date);

            //check if in not_vaccinated list
            if ( search_Bloom(*(virusNode->not_vacc_bloom), NUM_OF_HASHES, citizenID) == 1 ){

                    //search in list
                    if ( searchNotVacSkipList( virusNode->not_vacc, citizenID) == 1 ){
                        
                        //record found - remove
                        removeNotVacSkipList(&(virusNode->not_vacc), citizenID, &(virusNode->levels_notvac));
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






void populationStatus_all(struct List *VirusList, Hashtable citizens, CountryHash countries, char* virusname, char* date1, char* date2){


    if (countries == NULL || citizens == NULL || VirusList == NULL){
        printf("Empty!\n");
        return;
    }

    
    //for every country
    struct BucketCoun* co_temp;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        co_temp =countries[i].bucket;

        while ( co_temp != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                
                if (co_temp->element[j].name != NULL){


                    populationStatus_one(VirusList, citizens, countries, co_temp->element[j].name , virusname, date1, date2);


                } 

            }
            
            
            co_temp = co_temp->next_buc;

        
        }

    } 


            
}







void populationStatus_one(struct List *VirusList, Hashtable citizens, CountryHash countries, char* country, char* virusname, char* date1, char* date2){

    printf("Country: %s,  VirusName: %s,   Date1: %s,   Date2: %s\n", country, virusname, date1, date2);

    //get virus struct
    struct List* virusNode;
    virusNode = getelemfromlist(VirusList, virusname);

    if ( virusNode == NULL ) {printf("Error: no such virus\n"); return; }

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

        //if citizen is from this country
        if ( strcmp(cit_temp->country, country) == 0 ){
            if ( datecmp(date1, vac_temp->elem->date) >= 0 && datecmp( vac_temp->elem->date, date2 ) >= 0){
                num_of_vacc++;
            }
        }
        vac_temp = vac_temp->next;
    }

    struct Country* countryy = hashtable_getCoun(countries, country);

    //print results
    printf("%s %d %f%c\n", country, num_of_vacc, (float)num_of_vacc*100.0/(float)(countryy->population), '%' );


}


















void popStatusByAge_all(struct List *VirusList, Hashtable citizens, CountryHash countries, char* virusname, char* date1, char* date2){


    if (countries == NULL || citizens == NULL || VirusList == NULL){
        printf("Empty!\n");
        return;
    }

    //get virus struct
    struct List* virusNode;
    virusNode = getelemfromlist(VirusList, virusname);

    if ( virusNode == NULL ) {printf("Error: no such virus\n"); return; }


    
    //for every country
    struct BucketCoun* co_temp;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        co_temp =countries[i].bucket;

        while ( co_temp != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                
                if (co_temp->element[j].name != NULL){

                    popStatusByAge_one(VirusList, citizens, co_temp->element[j].name, virusname, date1, date2);

                } 

            }
            
            
            co_temp = co_temp->next_buc;

        
        }

    } 





            
}

















void popStatusByAge_one(struct List *VirusList, Hashtable citizens, char* country, char* virusname, char* date1, char* date2){


    //get virus struct
    struct List* virusNode;

    printf("Country: %s,  VirusName: %s,   Date1: %s,   Date2: %s\n", country, virusname, date1, date2);

    //get virus struct
    virusNode = getelemfromlist(VirusList, virusname);

    if ( virusNode == NULL ){ printf("Error: no such virus\n"); return; }

    VacSkipList vac_temp;
    char* citID_temp;
    struct Citizen *cit_temp;
    
    int vacc_0to19 = 0, total_0to19 = 0;
    int vacc_20to39 = 0, total_20to39 = 0;
    int vacc_40to59 = 0, total_40to59 = 0;
    int vacc_60plus = 0, total_60plus = 0;


    //VACCINATED LIST

    vac_temp = virusNode->vaccinated;

    while ( vac_temp->down != NULL){    //reach bottom
        vac_temp = vac_temp->down;
    }

    vac_temp = vac_temp->next;
    while ( vac_temp != NULL){

        citID_temp = vac_temp->elem->name;  //get citizen id
        cit_temp = hashtable_get(citizens, citID_temp); //get citizen

        //if citizen is from this country
        if ( strcmp(cit_temp->country, country) == 0 ){


            if ( cit_temp->age >= 0 && cit_temp->age < 20){

                total_0to19++;

                if ( datecmp(date1, vac_temp->elem->date) >= 0 
                    && datecmp( vac_temp->elem->date, date2 ) >= 0){
                    
                    vacc_0to19++;

                }

            }  else if ( cit_temp->age >= 20 && cit_temp->age < 40){
                
                total_20to39++;
                
                if ( datecmp(date1, vac_temp->elem->date) >= 0 
                    && datecmp( vac_temp->elem->date, date2 ) >= 0){
                    
                    vacc_20to39++;
                    
                }

            } else if ( cit_temp->age >= 40 && cit_temp->age < 60){
                
                total_40to59++;

                if ( datecmp(date1, vac_temp->elem->date) >= 0 
                    && datecmp( vac_temp->elem->date, date2 ) >= 0){
                    
                    vacc_40to59++;
                    
                }

            } else {

                total_60plus++;

                if ( datecmp(date1, vac_temp->elem->date) >= 0 
                    && datecmp( vac_temp->elem->date, date2 ) >= 0){
                    
                    vacc_60plus++;
                    
                }

            }




        }
        vac_temp = vac_temp->next;
    }


    //NOT VACCINATED LIST
    NotVacSkipList not_vac_temp;
    not_vac_temp = virusNode->not_vacc;

    while ( not_vac_temp->down != NULL){    //reach bottom
        not_vac_temp = not_vac_temp->down;
    }

    not_vac_temp = not_vac_temp->next;
    while ( not_vac_temp != NULL){

        citID_temp = not_vac_temp->elem->name;  //get citizen id
        cit_temp = hashtable_get(citizens, citID_temp); //get citizen

        //if citizen is from this country
        if ( strcmp(cit_temp->country, country) == 0 ){


            if ( cit_temp->age >= 0 && cit_temp->age < 20){

                total_0to19++;


            }  else if ( cit_temp->age >= 20 && cit_temp->age < 40){
                
                total_20to39++;
                

            } else if ( cit_temp->age >= 40 && cit_temp->age < 60){
                
                total_40to59++;


            } else {

                total_60plus++;

            }

        }
        not_vac_temp = not_vac_temp->next;
    }




    //print results
    printf("%s\n", country);
    int res = total_0to19;
    if ( res != 0 ){
        printf("0-19 %d %f%c\n", vacc_0to19, (float)vacc_0to19*100.0/(float)(total_0to19), '%' );
    } else {
        printf("0-19 No citizens\n");      
    }

    res = total_20to39;
    if ( res != 0 ){
        printf("20-39 %d %f%c\n", vacc_20to39, (float)vacc_20to39*100.0/(float)(total_20to39), '%' );
    } else {
        printf("20-39 No citizens\n");
    }

    res = total_40to59;
    if ( res != 0 ){
        printf("40-59 %d %f%c\n", vacc_40to59, (float)vacc_40to59*100.0/(float)(total_40to59), '%' );
    } else {
        printf("40-59 No citizens\n");
    }

    res = vacc_60plus;
    if ( res != 0 ){
        printf("60+ %d %f%c\n", vacc_60plus, (float)vacc_60plus*100.0/(float)(total_60plus), '%' );
    } else {
        printf("60+ No citizens\n");
    }  




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