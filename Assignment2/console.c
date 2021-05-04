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

    char input[300];
    char *arg;

    struct List* virusNode = NULL;
    char *virusName;
    
    while(1){

        printf("\nPlease give instructions\n");

        fgets(input,300, stdin);

        //printf("input: %s\n",input);
        

        //EXIT
        if ( strcmp( strtok(input, "\n"), "/exit") == 0){

        /*
            Έξοδος από την εφαρμογή. Το parent process στέλνει ένα SIGKILL 
            σήμα στους Monitors, τους περιμένει να τερματίσουν, και τυπώνει 
            σε ένα αρχείο με ονομασία log_file.χχχ όπου το χχχ είναι το 
            process ID του, το όνομα όλων των χωρών (των subdirectories) 
            που συμμετείχαν στην εφαρμογή με δεδομένα, το συνολικό αριθμό 
            αιτημάτων που δέχθηκε για είσοδο στις χώρες, και το συνολικό αριθμό 
            αιτημάτων που εγκρίθηκαν και απορρίφθηκαν. Πριν τερματίσει, θα 
            απελευθερώνει σωστά όλη τη δεσμευμένη μνήμη.
            Log file format (όπως και πιο πάνω):
            
            Italy China Germany
            TOTAL TRAVEL REQUESTS 883832 ACCEPTED 881818
            REJECTED 2014
        */
            return;
        }

        arg = strtok(input, " ");



        //TRAVEL REQUEST
        if ( strcmp(arg, "/travelRequest") == 0){


            //get arguments
            char *citizenID = strtok(NULL, " ");
            char* date = strtok(NULL, "\n");
            char *countryFrom= strtok(NULL, " ");
            char *countryTo = strtok(NULL, " ");            
            virusName = strtok(NULL, " ");
            

            //check for correct number of input
            if ( citizenID == NULL || date == NULL || countryFrom == NULL 
                ||  countryTo == NULL || virusName == NULL ){

                    printf("WRONG INPUT: must be 'ID Date CountryFrom CountryTo Virus\n");
                    continue;
                }



            // bloom filter = get from Monitor of Country

            struct BloomFilter *vacc_bloom = init_Bloom();
            VacSkipList vac_list = NULL;

            if ( search_Bloom(*vacc_bloom, NUM_OF_HASHES, citizenID) == 1){

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

            /*
               char *date;

               if ( getDate_VacSkipList(vac_list, citizenID) ){
                    
                   //check when vaccinated
                   if ( in_last_6(date) ){      //vaccinated in last 6 months
                        printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                   } else {
                        printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
                   }

               } else { //not in vaccinated skip list
                    printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
               }



            } else {    //not in the structure
                printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
            }
            */
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
