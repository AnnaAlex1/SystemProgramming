#include "threads_imp.h"
#include "records.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>



// MUTEXES
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t done_mutex = PTHREAD_MUTEX_INITIALIZER;


// CONDITION VARIABLES
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;


struct JobBuffer *jb = NULL;      // buffer queue


int done = 0;   // indication that all of input was given to buffer queue


void *thread_fun(void *argp){

    struct thread_args targs = *(struct thread_args *)argp;
    
    
    char *job;

    while ( 1 ){

        // check if done
        if ( done && job == NULL){
            break;
        }

        // get next job
        job = dequeue(jb, targs.cyclebuffersize);

        if ( job == NULL ){
            break;
        }

        pthread_cond_signal(&full_cond);


        // DO THE WORK FOR INPUT
        //printf("               Removing job %s to queue\n", job);
        if ( read_file(job, targs.citizens, targs.virus_list) == -1 ){
            perror("ERROR: Unsuccessful Reading!");
            exit(1);
        }
        free(job);
        

    }


    pthread_exit(0);

}






int thread_store_records(pthread_t *my_thr, int numofthreads, int cyclebuffersize, int numofinput,
                     Hashtable citizens, struct List** virus_list, CountryHash countries){


    int error;
    
    jb = buffer_init(cyclebuffersize);
    
    // THREADS ARGUMENTS
    struct thread_args targs;
    targs.citizens = citizens;
    targs.countries = countries;
    targs.virus_list = virus_list;
    targs.cyclebuffersize = cyclebuffersize;




    // Creation of Threads
    for ( int i=0; i<numofthreads; i++){

        error = pthread_create(my_thr+i, NULL, thread_fun, &targs);

        if (error != 0 ){
            strerror(error);
            exit(1);
        }

    }


    // FILL BUFFER
    fill_buffer(numofinput, cyclebuffersize, countries);

    return 0;

}




void thread_finish(int numofthreads, int cyclebuffersize, pthread_t * my_thr){

    int error;

    pthread_mutex_lock(&done_mutex);
    done = 1;
    pthread_cond_broadcast(&not_empty);     //wake up threads to finish
    pthread_mutex_unlock(&done_mutex);


    
    for ( int i=0; i<numofthreads; i++){
        
        //error = pthread_join(my_thr[i].thr, (void **) &status );
        error = pthread_join( *(my_thr+i), NULL );
        if ( error != 0){
            strerror(error);
            exit(1);
        }

    }


    pthread_mutex_destroy(&done_mutex);
    pthread_mutex_destroy(&work_mutex);
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&full_cond);
    buffer_destroy(&jb, cyclebuffersize);
    

}



int fill_buffer(int numofinput, int cyclicbuffersize, CountryHash countries){
    

    char* foldername;
    DIR *subdr;
    struct dirent *dfiles;

    char *filepath;



    struct Country* country_str;

    if (countries == NULL){
        printf("Hashtable is Empty!\n");
        return -1;
    }

    struct BucketCoun* current_buc;

    //SEARCH HASHTABLE OF COUNTRIES
    for (int i = 0; i < TABLE_SIZE; i++){

        current_buc = countries[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){ 

                if (current_buc->element[j].name != NULL){ //for every country

                    country_str = &(current_buc->element[j]);

                    //Find country's folder name
                    foldername = malloc( sizeof(char) * ( strlen("inputfolder") + strlen(country_str->name) + 2 ));

                    strcpy(foldername, "inputfolder");
                    strcat(foldername, "/");
                    strcat(foldername, country_str->name);


                    if ( (subdr = opendir(foldername)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   
                    country_str->num_of_files_read = 0;

                    //FOR EVERY FILE in the directory
                    while ( (dfiles = readdir(subdr)) != NULL ){

                        if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                            
                            //build path
                            filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(foldername) + 2 ) );         //+1 for the character '/'
                            strcpy(filepath, foldername);
                            strcat(filepath, "/");
                            strcat(filepath, dfiles->d_name);

                            //printf("file to open: %s\n", filepath);

                            
                            ///////////////////
                            //printf("Adding job: '%s' to queue\n", filepath);
                            enqueue(jb, filepath, cyclicbuffersize);

                            pthread_cond_signal(&not_empty);
                            ///////////////////////


                            free(filepath);

                            country_str->num_of_files_read++;
                            


                        }


                    }


                    closedir(subdr);
                    free(foldername);

                }
            }
            
            current_buc = current_buc->next_buc;  

        }
    
           
    } 


    return 0;

}









int fill_buffer_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries, int cyclicbuffersize){

    char* foldername;
    DIR *subdr;
    struct dirent *dfiles;

    int num_of_files = 0;
    int file_number;
    char *filepath;



    struct Country* country_str;

    if (countries == NULL){
        printf("Hashtable is Empty!\n");
        return -1;
    }

    struct BucketCoun* current_buc;

    //SEARCH HASHTABLE OF COUNTRIES
    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = countries[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){ 

                if (current_buc->element[j].name != NULL){ //for every country

                    country_str = &(current_buc->element[j]);

                    //Find country's folder name
                    foldername = malloc( sizeof(char) * ( strlen("inputfolder") + strlen(country_str->name) + 2 ));

                    strcpy(foldername, "inputfolder");
                    strcat(foldername, "/");
                    strcat(foldername, country_str->name);


                    if ( (subdr = opendir(foldername)) == NULL ) { perror("Sub-Directory cannot open!"); exit(1); }
   
                    //get previous number of files in country
                    num_of_files = country_str->num_of_files_read;

                    //FOR EVERY FILE in the directory
                    while ( (dfiles = readdir(subdr)) != NULL ){

                        if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                            
                            file_number = atoi( strchr(dfiles->d_name, '-') + 1);


                            //if filenumber greater than initial number of files in country
                            // => it's a new file
                            if ( file_number >= country_str->num_of_files_read){

                                //THIS IS A NEW FILE


                                //build path
                                filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(foldername) + 2 ) );         //+1 for the character '/'
                                strcpy(filepath, foldername);
                                strcat(filepath, "/");
                                strcat(filepath, dfiles->d_name);

                                ///////////////////
                                //printf("Adding job: '%s' to queue\n", filepath);
                                enqueue(jb, filepath, cyclicbuffersize);

                                //pthread_cond_broadcast(&not_empty);
                                pthread_cond_signal(&not_empty);


                                //printf("file to open: %s\n", filepath);


                                free(filepath);

                                num_of_files++;
                            }


                        }


                    }

                    country_str->num_of_files_read = num_of_files;

                    closedir(subdr);
                    free(foldername);

                }
            }
            
            current_buc = current_buc->next_buc;  

        }
    
           
    } 


    return 0;


}



















struct JobBuffer *buffer_init(int cyclebuffersize){

    struct JobBuffer *jb = malloc(sizeof(struct JobBuffer));
    jb->end = -1;
    jb->start = -1;
    jb->array = malloc(sizeof(char *) * cyclebuffersize);

    for (int i = 0; i < cyclebuffersize; i++){
        jb->array[i] = NULL;
    }
    
    return jb;


}





int enqueue( struct JobBuffer *b, char *job, int cyclebuffersize){
  

    if (pthread_mutex_lock(&queue_mutex)){          // lock
        exit(1);
    }


    while ( ((b->end + 1) %  cyclebuffersize) == b->start ){   // buffer is full

        //printf("Buffer is full. Waiting\n");
        pthread_cond_wait(&full_cond, &queue_mutex);

    }

    if ( b->start == -1 && b->end == -1){
        b->start = 0;
    }


    b->end = (b->end + 1) % cyclebuffersize;
    b->array[b->end] = malloc(sizeof(char*) * (strlen(job)+1));
    strcpy(b->array[b->end], job);
    


    if ( pthread_mutex_unlock(&queue_mutex)){       // unlock
        exit(1);
    }

    return 1;
    

}




char *dequeue( struct JobBuffer *b, int cyclebuffersize){



    if (pthread_mutex_lock(&queue_mutex)){
        exit(1);
    }


    while ( b->start == -1 && b->end == -1 ){   //case: empty

        if ( done ){                     //if there are no more jobs
            pthread_mutex_unlock(&queue_mutex);
            return NULL;
        }

        //printf("Buffer is empty. Waiting\n");
        pthread_cond_wait(&not_empty, &queue_mutex);
    }

    int ret_pos = b->start;


    if ( b->start == b->end ){  //case: only one item
        b->start = -1;
        b->end = -1;            //mark as empty
        
    } else {
        b->start = (b->start + 1) % cyclebuffersize;
    }


    char *ret_val = b->array[ret_pos];


    if ( pthread_mutex_unlock(&queue_mutex)){
        exit(1);
    }


    return ret_val;
}






void buffer_destroy(struct JobBuffer **jb, int cyclebuffersize){

    free((*jb)->array);
    free(*jb);
    
}