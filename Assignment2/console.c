//console.c


#include "console.h"
#include "virus.h"
#include "citizens.h"
#include "records.h"
#include "countries.h"
#include "sh_pipes.h"

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
                        rejected_req++;
                        
                        
                    } else {              //MAYBE IN THE STRUCTURE

                        //sent_message(); the whole line?

                        //get_message

                        /*if ( strcmp(message, "YES") == 0){
                            
                            //get date
                            if ( in_last_6(date) ){      //vaccinated in last 6 months
                                    printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                            } else {
                                    printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
                            }

                        } else if ( strcmp(message, "NO") == 0){
                            
                            printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
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


            if ( country == NULL ){     //country not given
            printf(" Country Not Given\n");
                /*
                    αριθμός πολιτών που έχουν ζητήσει έγκριση να ταξιδέψουν μεσα 
                    στο διαστημα [date1...date2] σε χώρες που ελέγχουν για εμβολιασμό 
                    κατά του virusName, και τον αριθμο πολιτών που εγκρίθηκαν και που 
                    απορρίφθηκαν. 
                */

            } else {        //country given

                printf("Country Given\n");
                /*
                    ίδια πληροφορία αλλά μόνο για τη συγκεκριμένη χώρα country. 
                */

                /*
            
                Output format: Παράδειγμα:
                TOTAL REQUESTS 29150
                ACCEPTED 25663
                REJECTED 3487
                */

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













int datecmp(char* date1, char* date2){

    char vac_date[12];
    strcpy(vac_date, date1);
    char cur_date[12];
    strcpy(cur_date, date2);



    int day_vac = atoi(strtok(vac_date, "-"));
    int month_vac = atoi(strtok(NULL, "-"));
    int year_vac = atoi(strtok(NULL, "\0"));

    int day_cur = atoi(strtok(cur_date, "-"));
    int month_cur = atoi(strtok(NULL, "-"));
    int year_cur = atoi(strtok(NULL, "\0"));

    

    if ( year_cur > year_vac){   //if year_cur > year_vac

        return 1;

    } else if ( year_vac > year_cur ){   //year_vac > year_cur

        return -1;

    } else {    //case: year_vac == year_cur

        if ( month_cur > month_vac ){   // month_cur > month_vac

            return 1;

        } else if ( month_vac > month_cur ){   // month_vac > month_cur

            return -1;

        } else {    //case: month_vac == month_cur
            
            if ( day_cur > day_vac ){   //if day_cur > day_vac

                return 1;

            } else if ( day_vac > day_cur ){   // day_vac > day_cur

                return -1;

            } else {    //case: day_vac == day_cur

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
