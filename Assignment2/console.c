//console.c


#include "console.h"
#include "virus.h"
#include "citizens.h"
#include "records.h"
#include "countries.h"
#include "sh_pipes.h"
#include "requests.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> 
#include <sys/wait.h>
#include <unistd.h>


extern int numMonitors;

extern int signal_num;


void console( struct MonitorStruct *commun, CountryMainHash countries, size_t bufferSize){

    char input[300];
    char *arg;

    char *virusName;

    int total_requests=0, accepted_req=0, rejected_req=0;
    int approved = 0;
    
    struct RequestsList *req_list = NULL;



    while(1){

        printf("\nPlease give instructions\n");

        fgets(input,300, stdin);

        //printf("input: %s\n",input);
        

        //EXIT
        if ( (strcmp( strtok(input, "\n"), "/exit") == 0) || signal_num == 4 ){               //EXIT

            int status;
            

            //SEND SIGKILL TO MONITORS
            for (int i=0; i<numMonitors; i++){
                kill(commun[i].pid, SIGKILL);
            }

            //WAIT FOR ALL MONITORS TO TERMINATE
            for (int i = 0; i < numMonitors; i++){
                    waitpid(commun[i].pid, &status, 0);
                    printf("PID: %d, status: %d\n",commun[i].pid, status);
                    printf("WTERMSIG(status)= %d\n", WTERMSIG(status));
                    printf("WIFEXITED(status)= %d\n", WIFEXITED(status));
            }
            

            
            //RELEASE SOURCES

            printf("\nFinished with Pipes\n");
            for (int i = 0; i < numMonitors; i++) {
                close(commun[i].fd_w);
                close(commun[i].fd_r);
                unlink(commun[i].fifoname_w);
                unlink(commun[i].fifoname_r);

                free(commun[i].fifoname_w);
                free(commun[i].fifoname_r);

                //deleteVirMain(&(commun[i].viruses));
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
            print_hashtableCounMain(countries, logfile);

            fprintf(logfile, "%d\n%d\n%d", total_requests, accepted_req, rejected_req);
            

            //FREE MEMORY
            deleteReqlist(&req_list);

            return;
        }

        
        arg = strtok(input, " ");




        //TRAVEL REQUEST
        if ( strcmp(arg, "/travelRequest") == 0){

            

            //get arguments
            char *citizenID = strtok(NULL, " ");
            char *date = strtok(NULL, "\n");
            char *countryFrom= strtok(NULL, " ");
            char *countryTo = strtok(NULL, " ");            
            virusName = strtok(NULL, " ");
            

            //check for correct number of input
            if ( citizenID == NULL || date == NULL || countryFrom == NULL 
                ||  countryTo == NULL || virusName == NULL ){

                    printf("WRONG INPUT: must be 'ID Date CountryFrom CountryTo Virus\n");
                    continue;
                }

            


            //FIND MONITOR THAT HANDLES COUNTRYFROM
            struct CountryMain* cs = hashtable_getCounMain(countries, countryFrom);
            
            for (int i = 0; i < numMonitors; i++){
                
                if ( commun[i].pid == cs->pid ){    //found monitor that handled countryfrom


                    //find virus struct

                    struct VirusesListMain *vir_elem;
                    vir_elem = getelemfromVirMain(commun[i].viruses, virusName);


                    //check bloomfilter

                    if ( search_Bloom(*(vir_elem->vacc_bloom), NUM_OF_HASHES, citizenID) == 0){//NOT IN THE STRUCTURE
                        
                        printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
                        approved = 0;
                        rejected_req++;
                        
                        
                    } else {              //MAYBE IN THE STRUCTURE

                        //sent_message(); the whole line?

                        //get_message

                        /*if ( strcmp(message, "YES") == 0){
                            
                            //get date
                            if ( in_last_6(date) ){      //vaccinated in last 6 months
                                    printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                                    approved = 1;
                            } else {
                                    printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
                                    approved = 0;
                            }

                        } else if ( strcmp(message, "NO") == 0){
                            
                            printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
                            approved = 0;
                            rejected_req++;

                        }*/



                        //ζητάει μέσω named pipe από το Monitor process που διαχειρίζεται 
                        //τη χώρα countryFrom αν όντως έχει εμβολιαστεί ο citizenID.
                        /*
                        Αν το bloom filter υποδεικνύει πως ο πολίτης citizenID ίσως έχει εμβολιαστεί κατά 
                        του virusName, η εφαρμογη  Το Monitor process απαντάει μέσω named pipe YES/NO 
                        όπου στη περίπτωση του YES, στέλνει και την ημερομηνία εμβολιασμού.  Η εφαρμογή 
                        ελέγχει αν έχει εμβολιαστεί ο πολίτης λιγότερο από 6 
                        μήνες πριν την επιθυμητή ημερομηνία ταξιδιού date και τυπώνει ένα από τα ακόλουθα 
                        μηνύματα

                        */

                    }

                }
                

            }
            

            //ADD REQUEST TO LIST
            addRequest(&req_list, virusName, date, countryTo, approved);

        //TRAVEL STATE
        } else if ( strcmp(arg, "/travelStats") == 0){

           
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
        } else if ( strcmp(arg, "/addVaccinationRecords") == 0){
            /*
            Με αυτό το αίτημα ο χρήστης έχει τοποθετήσει στο input_dir/country 
            ένα ή περισσότερα αρχεία για επεξεργασία από το Monitor process. Το 
            parent process στέλνει ειδοποίηση μέσω SIGUSR1 σήμα στο  Monitor 
            process που διαχειρίζεται τη χώρα country ότι υπάρχουν input files 
            για ανάγνωση στον κατάλογο. Το Monitor process διαβάζει ότι νέο αρχείο 
            βρει, ενημερώνει τις δομές δεδομένων και στέλνει πίσω στο parent process, 
            μέσω named pipe, τα ενημερωμένα του bloom filters που αναπαριστούν 
            το σύνολο πολιτών που έχουν εμβολιαστεί           
            */

            char *country = strtok(NULL, "\n");   


            //FIND MONITOR THAT HANDLES COUNTRYFROM
            struct CountryMain* cs = hashtable_getCounMain(countries, country);
            
            for (int i = 0; i < numMonitors; i++){
                
                if ( commun[i].pid == cs->pid ){    //found monitor that handled countryfrom

                    kill(commun[i].pid, SIGUSR1);

                }


                //get_bloomfilters();
            }

            



        //SEARCH VACCINATION STATUS 
        } else if ( strcmp(arg, "/searchVaccinationStatus") == 0){

            /*
            To parent process προωθεί σε όλους τα Monitor processes το αίτημα 
            μέσω named pipes. Το Monitor process που διαχειρίζεται τον πολίτη 
            με αναγνωριστικό citizenID στέλνει μέσω named pipe ό,τι πληροφορίες 
            έχει για τους εμβολιασμούς που έχει κάνει/δεν έχει κάνει ο συγκεκριμένος 
            πολίτης. Όταν λάβει τις πληροφορίες, το parent τις τυπώνει στο stdout.
            Output format: Παράδειγμα:

            889 JOHN PAPADOPOULOS GREECE AGE 52
            COVID-19 VACCINATED ON 27-12-2020 H1N1 NOT YET VACCINATED
            */

            char *citizenID = strtok(NULL, "\n");      

            for (int i = 0; i < numMonitors; i++){
               sent_message_wrong(commun[i].fd_w, citizenID, bufferSize);
            }

            //get_message_wrong();
            //get info
            //get vaccinations
            //print



        } 




    }   //end of while


    
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





int in_last_six(char *date){

    char vac_date[12];
    strcpy(vac_date, date);
    char cur_date[12];
    strcpy(cur_date, get_cur_date());


    int day_vac = atoi(strtok(vac_date, "-"));
    int month_vac = atoi(strtok(NULL, "-"));
    int year_vac = atoi(strtok(NULL, "\0"));

    int day_cur = atoi(strtok(cur_date, "-"));
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
