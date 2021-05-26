//console.c


#include "console.h"
#include "virus.h"
#include "citizens.h"
#include "records.h"
#include "countries.h"
#include "sh_sockets.h"
#include "requests.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> 
#include <sys/wait.h>
#include <unistd.h>


extern struct Arguments arg;

extern int signal_num;




void console( struct MonitorStruct *commun, CountryMainHash countries, size_t socketBufferSize){
/*
    char input[500];
    char *arg_read;

    char *virusName;

    int accepted_req=0, rejected_req=0;
    
    struct RequestsList *req_list = NULL;

    char *first_word;

    while(1){

        printf("\nPlease give instructions\n");

        if (signal_num == 0){   // no signal received
            fgets(input,500, stdin);
        }


        //EXIT
        first_word = strtok(input, "\n");
        if ( first_word == NULL){
            continue;
        }




        //SIGCHLD

        if ( signal_num == 1){      //case: SIGCHLD

            for (int i = 0; i < arg.numMonitors; i++){

                if ( commun[i].sock == -1 ){    // found info of dead child

                    int prev_pid = commun[i].pid;

                    create_child(i);    //create new process

                    //re-assign countries to new child
                    reassign_countries(commun, countries, prev_pid, i, socketBufferSize);

                    //get bloomfilters
                    if (get_bloomfilters(commun, socketBufferSize, arg.numMonitors, commun[i].fd_r, 0) == -1){
                        perror("ERROR in getting bloomfilters");
                        exit(1);
                    }

                    char *ready = get_message(commun[i].fd_r, socketBufferSize);
                    if ( strcmp(ready, "READY") != 0 ){
                        printf("Something Went Wrong!\n");
                    } else {
                        printf("Monitor %d is ready!\n", commun[i].pid);
                    }
                    
                    free(ready);

                }
                
            }
            

            signal_num = 0;

            continue; 
        }



        if ( (strcmp( first_word, "/exit") == 0) || signal_num == 4 ){        //EXIT

            int status;
            
            signal_num = -1;        // to avoid creation of new child process

            //SEND SIGKILL TO MONITORS
            for (int i=0; i < arg.numMonitors; i++){
                kill(commun[i].pid, SIGKILL);
            }

            //WAIT FOR ALL MONITORS TO TERMINATE
            for (int i = 0; i < arg.numMonitors; i++){
                waitpid(commun[i].pid, &status, 0);
            }
            

            
            //RELEASE SOURCES

            printf("\nFinished with Pipes\n");
            for (int i = 0; i < arg.numMonitors; i++) {
                close(commun[i].fd_w);
                close(commun[i].fd_r);
                unlink(commun[i].fifoname_w);
                unlink(commun[i].fifoname_r);

                deleteVirMain(&(commun[i].viruses));
            }
            free(commun);
            
    
            //PRINT LOGFILE
            int my_pid = getpid();
            char pid_str[7];
            sprintf(pid_str, "%d", my_pid);

            char *filename = malloc( sizeof(char) * (strlen("logfiles/log_file.")+strlen(pid_str)+1) );
            strcpy(filename, "logfiles/log_file.");
            strcat(filename, pid_str);

            FILE* logfile = fopen(filename, "w");
            if ( logfile == NULL){
                perror("ERROR in opening file 'LOGFILE'");
                return;
            }

            //print in log_file
            print_hashtableCounMain(countries, logfile);
            fprintf(logfile, "%d\n%d\n%d", accepted_req+rejected_req, accepted_req, rejected_req);
            

            //FREE MEMORY
            fclose(logfile);
            free(filename);
            deleteReqlist(&req_list);

            return;
        }

        
        arg_read = strtok(input, " ");




        //TRAVEL REQUEST
        if ( strcmp(arg_read, "/travelRequest") == 0){

            

            //get arguments
            char *citizenID = strtok(NULL, " ");
            char *date = strtok(NULL, " ");
            char *countryFrom= strtok(NULL, " ");
            char *countryTo = strtok(NULL, " ");            
            virusName = strtok(NULL, "\n");
            

            //check for correct number of input
            if ( citizenID == NULL || date == NULL || countryFrom == NULL 
                ||  countryTo == NULL || virusName == NULL ){

                    printf("WRONG INPUT: must be 'ID Date CountryFrom CountryTo Virus\n");
                    continue;
                }

            

            travel_Req(commun, countries, socketBufferSize, virusName, countryFrom, citizenID, 
                    date, countryTo, &rejected_req, &accepted_req, &req_list);



        //TRAVEL STATE
        } else if ( strcmp(arg_read, "/travelStats") == 0){

           
            virusName = strtok(NULL, " ");
            char *date1 = strtok(NULL, " ");
            char *date2 = strtok(NULL, "\n");        //handling case country is missing
            date2 = strtok(date2, " ");
            char* country = strtok(NULL, "\n");

            //check for correct number of input
            if ( date1 == NULL || date2 == NULL || virusName == NULL ){

                printf("WRONG INPUT: must be 'VirusName Date1 Date2 [Country]\n");
                continue;
            }

            if ( country == NULL ){
                travel_stat_all(req_list, countries, virusName, date1, date2);
            } else {
                travel_stat_one(req_list, virusName, date1, date2, country);
            }




        //ADD VACCINATION RECORDS
        } else if ( strcmp(arg_read, "/addVaccinationRecords") == 0){

            char *country = strtok(NULL, "\n");   

            if ( country == NULL){
                printf("Please specify Country!\n");
                continue;
            }

            //FIND MONITOR THAT HANDLES COUNTRYFROM
            struct CountryMain* cs = hashtable_getCounMain(countries, country);
            
            if ( cs == NULL ){
                printf("No such Country!\n");
                continue;
            }


            for (int i = 0; i < arg.numMonitors; i++){
                
                if ( commun[i].pid == cs->pid ){    //found monitor that handled countryfrom

                    //send SIGUSR1 signal
                    kill(commun[i].pid, SIGUSR1);

                    //wait for new bloomfilter
                    if ( get_bloomfilters(commun, socketBufferSize, arg.numMonitors, i, 1) == -1){
                        perror("ERROR in replacing bloomfilter");
                        exit(1);
                    }

                    break;
                }


            }

            



        //SEARCH VACCINATION STATUS 
        } else if ( strcmp(arg_read, "/searchVaccinationStatus") == 0){

            int found = 0;

            char *citizenID = strtok(NULL, "\n"); 

            if (citizenID == NULL){
                printf("Please give a CitizenID\n");
                continue;
            }     

            for (int i = 0; i < arg.numMonitors; i++){

                kill(commun[i].pid, SIGUSR2);   //SEND SIGNAL FOR READING

                //send command
                send_message(commun[i].sock, "searchVaccinationStatus", strlen("searchVaccinationStatus")+1, socketBufferSize);
                

                //send citizenID
                send_message(commun[i].sock, citizenID, strlen(citizenID)+1, socketBufferSize);
            }

            char *answer;

            for (int i = 0; i < arg.numMonitors; i++){
               
                answer = get_message(commun[i].sock, socketBufferSize);
            
                if ( strcmp(answer, "FOUND") == 0 ){

                    found = 1;

                    //wait for information
                    char *info;

                    while ( 1 ){

                        info = get_message(commun[i].sock, socketBufferSize);

                        if ( strcmp(info, "DONE") == 0 ){
                            free(info);
                            break;
                        }

                        printf("%s\n", info);

                        free(info);
                    }


                }

                free(answer);
            }

            if ( !found ){
                printf("No such Citizen!\n");
            }




        } 




    }   //end of while


 */   
}




void travel_Req(struct MonitorStruct *commun, CountryMainHash countries, size_t socketBufferSize,
                char *virusName, char *countryFrom, char *citizenID, char *trav_date, char* countryTo,
                 int *rejected_req, int *accepted_req, struct RequestsList **req_list){

    int approved = 0;

    //FIND MONITOR THAT HANDLES COUNTRYFROM
    struct CountryMain* cs = hashtable_getCounMain(countries, countryFrom);

    if ( cs == NULL ){
        printf("No such Country: %s!\n", countryFrom);
        return;
    }
    
    for (int i = 0; i < arg.numMonitors; i++){

        
        if ( commun[i].pid == cs->pid ){    //found monitor that handled countryfrom

            //find virus struct
            struct VirusesListMain *vir_elem;
            vir_elem = getelemfromVirMain(commun[i].viruses, virusName);

            if ( vir_elem == NULL ){
                //No such virus in this Monitor
                printf("REQUEST REJECTED - YOU ARE NOT VACCINATED (NO VIRUS)\n");
                approved = 0;
                (*rejected_req)++;                
            } else 

            //check bloomfilter

            if ( search_Bloom(*(vir_elem->vacc_bloom), NUM_OF_HASHES, citizenID) == 0){//NOT IN THE STRUCTURE
                
                printf("REQUEST REJECTED - YOU ARE NOT VACCINATED (BLOOM)\n");
                approved = 0;
                (*rejected_req)++;
                
            } else {              //MAYBE IN THE STRUCTURE

                //SEND SIGNAL FOR READING
                kill(commun[i].pid, SIGUSR2);

                //SEND CHOSEN COMMAND
                send_message(commun[i].sock, "travelRequest", strlen("travelRequest")+1, socketBufferSize);


                //SEND NECESSARY INFO
                //send name of virus
                send_message(commun[i].sock, virusName, strlen(virusName)+1, socketBufferSize);
                //send citizenID
                send_message(commun[i].sock, citizenID, strlen(citizenID)+1, socketBufferSize);
                

                //GET RESULT
                char *result = get_message(commun[i].sock, socketBufferSize);

                if ( strcmp(result, "YES") == 0){
                    
                    //GET DATE
                    char *vaccin_date = get_message(commun[i].sock, socketBufferSize);

                    int all = in_prev_six(vaccin_date, trav_date);
                    
                    if ( all == -1 ){   //wrong info
                        printf("Wrong Date!\n");
                    } else if ( all == 1 ){      //vaccinated in previous 6 months
                            printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                            approved = 1;
                            (*accepted_req)++;
                    } else {
                            printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
                            approved = 0;
                            (*rejected_req)++;
                    }

                    free(vaccin_date);

                } else if ( strcmp(result, "NO") == 0){
                    
                    printf("REQUEST REJECTED - YOU ARE NOT VACCINATED (SKIPLIST)\n");
                    approved = 0;
                    (*rejected_req)++;

                }

                free(result);


            }

            break;
        }
        

    }
    

    //ADD REQUEST TO LIST
    addRequest(req_list, virusName, trav_date, countryTo, approved);


}



void travel_stat_all(struct RequestsList *req, CountryMainHash countries, char *virus, char* date1, char* date2){

    if (countries == NULL){
        printf("No Countries -> No Requests!\n");
        return;
    }

    struct BucketCounMain* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = countries[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                
                if (current_buc->element[j].name != NULL){

                    printf("\n%s\n", current_buc->element[j].name);
                    travel_stat_one(req, virus, date1, date2, current_buc->element[j].name);

                }
            }
            
            
            current_buc = current_buc->next_buc;

            

        }
           
    }
}




void travel_stat_one(struct RequestsList *req, char *virus, char* date1, char* date2, char* country){

    int num_of_req = 0;
    int approved = 0, rejected = 0;


    while( req != NULL){

        //if citizen is from this Country and for this virus
        if ( strcmp(virus, req->virus) == 0 && strcmp(req->country, country) == 0 ){  

            //if it's between [date1, date2]
            if ( datecmp(date1, req->date) != -1 && datecmp(req->date, date2) != -1){

                if (req->approved == 1){
                    approved++;
                } else {
                    rejected++;
                }

                num_of_req++;
            }

        }

        req = req->next;
    }

    
    printf("TOTAL REQUESTS: %d\n", num_of_req);
    printf("ACCEPTED: %d\n", approved);
    printf("REJECTED: %d\n", rejected);


}












//DECIDES THE LATEST DATE
int datecmp(char* date1, char* date2){

    char firstdate[12];
    strcpy(firstdate, date1);
    char cur_date[12];
    strcpy(cur_date, date2);



    int day_first = atoi(strtok(firstdate, "-"));
    int month_first = atoi(strtok(NULL, "-"));
    int year_first = atoi(strtok(NULL, "\0"));

    int day_sec = atoi(strtok(cur_date, "-"));
    int month_sec = atoi(strtok(NULL, "-"));
    int year_sec = atoi(strtok(NULL, "\0"));

    

    if ( year_sec > year_first){   //check years

        return 1;

    } else if ( year_first > year_sec ){   

        return -1;

    } else {    //case: year_fir == year_sec

        if ( month_sec > month_first ){   // check months

            return 1;

        } else if ( month_first > month_sec ){   

            return -1;

        } else {    //case: month_first == month_sec
            
            if ( day_sec > day_first ){   //check days

                return 1;

            } else if ( day_first > day_sec ){

                return -1;

            } else {    //case: day_first == day_sec

                return 0;

            }

        }

    }

    return 0;

}







//RETURNS CURRENT DATE
char *get_cur_date(){

    time_t cur_time;

    time(&cur_time);


    struct tm *arith = localtime(&cur_time);
    char *cur_date = malloc( sizeof(char) * 11); //format: dd-mm-yyyy +\0
 
    char buffer[40];
    
    snprintf(buffer, 40, "%d-%d-%d", arith->tm_mday, arith->tm_mon+1, arith->tm_year + 1900);
    strcpy( cur_date, buffer);

    return cur_date;
}




//DECIDES IF DATE WAS IN LAST SIX MONTHS
int in_prev_six(char *date1, char *date2){

    char vac_date[12];
    strcpy(vac_date, date1);

    char trav_date[12];
    strcpy(trav_date, date2);


    int day_vac = atoi(strtok(vac_date, "-"));
    int month_vac = atoi(strtok(NULL, "-"));
    int year_vac = atoi(strtok(NULL, "\0"));

    int day_cur = atoi(strtok(trav_date, "-"));
    int month_cur = atoi(strtok(NULL, "-"));
    int year_cur = atoi(strtok(NULL, "\0"));

    

    if ( year_cur < year_vac ){  // vaccinated in the future -> false
        return -1;
    }


    if ( year_cur > year_vac){   // vaccination in different year

        month_cur += 12;
    }


    if ( month_cur - month_vac > 6 ){   //months more than 6 
        return 0;
    } else if ( month_cur - month_vac == 6 ){       //months are 6

        if ( day_cur > day_vac ){   // earlier in the month
            return 0;
        }

    }

    return 1;

 


}



void reassign_countries(struct MonitorStruct *commun, CountryMainHash ht, int prev_pid, int com_pos, size_t socketBufferSize){


    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return;
    }

    char* foldername;

    struct BucketCounMain* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++){   
        
        current_buc = ht[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                
                if (current_buc->element[j].name != NULL){


                    if ( current_buc->element[j].pid == prev_pid ){ //found a country that was previously assigned to this


                        //Find country's folder name
                        foldername = malloc( sizeof(char) * ( strlen("inputfolder") + strlen(current_buc->element[j].name) + 2 ));

                        strcpy(foldername, "inputfolder");
                        strcat(foldername, "/");
                        strcat(foldername, current_buc->element[j].name);

                        //send foldername
                        send_message(commun[com_pos].sock, foldername, strlen(foldername)+1, socketBufferSize);

                        //change pid in country_struct
                        current_buc->element[j].pid = commun[com_pos].pid;
                        
                        free(foldername);
                    }


                }
            }
            
            
            current_buc = current_buc->next_buc;

            

        }
           
    } 

    printf("\nFinished Distribution\n");
    char *mes = malloc(socketBufferSize);
    strcpy(mes, "DONE");
    send_message(commun[com_pos].sock, mes, strlen("DONE")+1, socketBufferSize);

    free(mes);

}