#include "countries.h"
#include "citizens.h"
#include "records.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



CountryHash hashtable_createCoun(){
    
    CountryHash ht;

    ht = malloc(sizeof(struct TablePosCoun)*TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        ht[i].bucket = NULL;
    }
    

    return ht;
}




void hashtable_destroyCoun(CountryHash ht){

    if (ht == NULL){
        return;
    }

    printf("Destroying the Hashtable...\n");


    struct BucketCoun* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        

        while (ht[i].bucket != NULL){

            current_buc = ht[i].bucket;

            for (int j = 0; j < BUC_SIZE; j++){
                free(current_buc->element[j].name);
            }
            
            free(current_buc->element);

            ht[i].bucket = current_buc->next_buc;

            free(current_buc);

        }
           
    }
    
}





struct BucketCoun* bucket_createCoun(){
    struct BucketCoun* new_Bucket;

    new_Bucket = malloc(sizeof(struct BucketCoun));
    
    new_Bucket->element = malloc(sizeof(struct Country)*BUC_SIZE );
    new_Bucket->cur_size=0;
    new_Bucket->next_buc=NULL;

    for (int i = 0; i < BUC_SIZE; i++){
        new_Bucket->element[i].name = NULL;
        new_Bucket->element[i].num_of_files_read = 0;
    }
    

    return new_Bucket;
}






void hashtable_addCoun(CountryHash ht, char* coun_name){
    
    struct Country* new_coun;
    new_coun = malloc(sizeof(struct Country));
    new_coun->name = malloc(sizeof(char) * (strlen(coun_name)+1));
    strcpy(new_coun->name, coun_name);
    new_coun->num_of_files_read = 0;
    

    int pos = hashfunction(coun_name);  //get row to be put

    struct BucketCoun* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){    //if row is empty
        ht[pos].bucket = bucket_createCoun();
        bucket = ht[pos].bucket;
        memcpy( &bucket->element[bucket->cur_size], new_coun, sizeof(struct Country) );
        bucket->cur_size++; 
    } else {
        //find next empty position
        bucket = ht[pos].bucket;
        while (bucket->next_buc != NULL && bucket->cur_size == BUC_SIZE){
            bucket = bucket->next_buc;
        }

        if (bucket->next_buc == NULL ){  //case: no empty bucket, need to create another
            
            bucket->next_buc = bucket_createCoun();
            memcpy( &bucket->element[bucket->cur_size], new_coun, sizeof(struct Country) );
            bucket->cur_size++;   

        } else if (bucket->cur_size < BUC_SIZE){        //case: found a bucket with space
            
            
            for (int j=0; j<BUC_SIZE; j++){
                if (bucket->element[j].name == NULL){
                memcpy( &bucket->element[bucket->cur_size], new_coun, sizeof(struct Country) );
                bucket->cur_size++; 
                    break;
                }
            }

        }
    }

    free(new_coun);


}



int hashtable_findCoun(CountryHash ht, char* country_name){

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return 0;
    }

    int pos = hashfunction(country_name);   //find row

    struct BucketCoun* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){    //row is empty -> not found
        return 0;
    } else {

        bucket = ht[pos].bucket;
        while (bucket != NULL){

            for (int i = 0; i < BUC_SIZE; i++)
            {
                if (bucket->element[i].name != NULL){    //if position is not empty

                    if ( strcmp(bucket->element[i].name, country_name ) == 0 ){    //if found
                        return 1;
                    }

                }
            }
            
            bucket = bucket->next_buc;
        }

    }

    return 0;
}




struct Country* hashtable_getCoun(CountryHash ht, char* country_name){

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return NULL;
    }

    int pos = hashfunction(country_name);

    struct BucketCoun* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){
        return NULL;
    } else {

        bucket = ht[pos].bucket;
        while (bucket != NULL){

            for (int i = 0; i < BUC_SIZE; i++)
            {
                if (bucket->element[i].name != NULL){    //if position is not empty

                    if ( strcmp(bucket->element[i].name, country_name ) == 0 ){    //if found
                        return &(bucket->element[i]);
                    }

                }
            }
            


            bucket = bucket->next_buc;
        }

    }
    return NULL;
}




void set_num_of_files(CountryHash ht, char* country_name, int num_of_files){

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return;
    }

    int pos = hashfunction(country_name);

    struct BucketCoun* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){
        return;
    } else {

        bucket = ht[pos].bucket;
        while (bucket != NULL){

            for (int i = 0; i < BUC_SIZE; i++)
            {
                if (bucket->element[i].name != NULL){    //if position is not empty

                    if ( strcmp(bucket->element[i].name, country_name ) == 0 ){    //if found
                        bucket->element[i].num_of_files_read = num_of_files;
                    }

                }
            }
            


            bucket = bucket->next_buc;
        }

    }


    return; 


}




void print_hashtableCoun(CountryHash ht, FILE* stream){

    printf("\n");

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return;
    }

    struct BucketCoun* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = ht[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                
                if (current_buc->element[j].name != NULL){

                    if (stream == NULL){
                        fprintf(stdout, "%s\n", current_buc->element[j].name);
                    } else {
                        fprintf(stream, "%s\n", current_buc->element[j].name);
                    }


                }
            }
            
            
            current_buc = current_buc->next_buc;

            

        }
           
    } 
}





///////////////////////////////////////////////////////////////////////////




CountryMainHash hashtable_createCounMain(){

    CountryMainHash ht;

    ht = malloc(sizeof(struct TablePosCounMain)*TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        ht[i].bucket = NULL;
    }
    

    return ht;

}



struct BucketCounMain* bucket_createCounMain(){
    
    struct BucketCounMain* new_Bucket;

    new_Bucket = malloc(sizeof(struct BucketCounMain));
    
    new_Bucket->element = malloc(sizeof(struct CountryMain)*BUC_SIZE );
    new_Bucket->cur_size=0;
    new_Bucket->next_buc=NULL;

    for (int i = 0; i < BUC_SIZE; i++){
        new_Bucket->element[i].name = NULL;
        new_Bucket->element[i].pid = -1;
    }
    

    return new_Bucket;
}




void hashtable_addCounMain(CountryMainHash ht, char* coun_name, int pid){
    
    struct CountryMain* new_coun;
    new_coun = malloc(sizeof(struct CountryMain));
    new_coun->name = malloc(sizeof(char) * (strlen(coun_name)+1));
    strcpy(new_coun->name, coun_name);
    new_coun->pid = pid;


    int pos = hashfunction(coun_name);  //get row to be put

    struct BucketCounMain* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){    //if row is empty
        ht[pos].bucket = bucket_createCounMain();
        bucket = ht[pos].bucket;
        memcpy( &bucket->element[bucket->cur_size], new_coun, sizeof(struct CountryMain) );
        bucket->cur_size++; 
    } else {
        //find next empty position
        bucket = ht[pos].bucket;
        while (bucket->next_buc != NULL && bucket->cur_size == BUC_SIZE){
            bucket = bucket->next_buc;
        }

        if (bucket->next_buc == NULL ){  //case: no empty bucket, need to create another
            
            bucket->next_buc = bucket_createCounMain();
            memcpy( &bucket->element[bucket->cur_size], new_coun, sizeof(struct CountryMain) );
            bucket->cur_size++;   

        } else if (bucket->cur_size < BUC_SIZE){        //case: found a bucket with space
            
            
            for (int j=0; j<BUC_SIZE; j++){
                if (bucket->element[j].name == NULL){
                memcpy( &bucket->element[bucket->cur_size], new_coun, sizeof(struct CountryMain) );
                bucket->cur_size++; 
                    break;
                }
            }

        }
    }

    free(new_coun);

}




struct CountryMain* hashtable_getCounMain(CountryMainHash ht, char* country_name){

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return NULL;
    }

    int pos = hashfunction(country_name);

    struct BucketCounMain* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){
        return NULL;
    } else {

        bucket = ht[pos].bucket;
        while (bucket != NULL){

            for (int i = 0; i < BUC_SIZE; i++)
            {
                if (bucket->element[i].name != NULL){    //if position is not empty

                    if ( strcmp(bucket->element[i].name, country_name ) == 0 ){    //if found
                        return &(bucket->element[i]);
                    }

                }
            }
            


            bucket = bucket->next_buc;
        }

    }
    return NULL;
}



void hashtable_destroyCounMain(CountryMainHash ht){
   
    if (ht == NULL){
        return;
    }

    printf("Destroying the Hashtable...\n");


    struct BucketCounMain* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        

        while (ht[i].bucket != NULL){

            current_buc = ht[i].bucket;

            for (int j = 0; j < BUC_SIZE; j++){
                free(current_buc->element[j].name);
            }
            
            free(current_buc->element);

            ht[i].bucket = current_buc->next_buc;

            free(current_buc);

        }
           
    }

}



void print_hashtableCounMain(CountryMainHash ht, FILE* stream){
    
    printf("\n");

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return;
    }

    struct BucketCounMain* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = ht[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                
                if (current_buc->element[j].name != NULL){

                    if (stream == NULL){
                        fprintf(stdout, "%s\n", current_buc->element[j].name);
                    } else {
                        fprintf(stream, "%s\n", current_buc->element[j].name);
                    }


                }
            }
            
            
            current_buc = current_buc->next_buc;

            

        }
           
    } 
}




///////////////////////////////////////////////



void addinFilesList(struct CountryFiles** fileslist, char* filename){

    struct CountryFiles *newnode;
    newnode = malloc(sizeof(struct CountryFiles));
    newnode->filename = malloc(sizeof(char)* (strlen(filename)+1));
    memcpy(newnode->filename, filename, sizeof(char)* (strlen(filename)+1) );
    
    newnode->next = NULL;


    struct CountryFiles *temp;

    if (*fileslist == NULL){
        *fileslist = newnode;
    } else {
        temp = *fileslist;

        //place at the beginning:
        *fileslist = newnode;
        newnode->next = temp;
        
    }

    return;
}






int deleteFileslist(struct CountryFiles** fileslist){

    printf("Deleting List of Filenames...\n");
    if ( fileslist == NULL || *fileslist == NULL) return -1;

    struct CountryFiles* to_free;

    while(*fileslist != NULL){
        to_free = *fileslist;
        *fileslist = (*fileslist)->next;
        free(to_free->filename);
        free(to_free);
    }

    return 0;
}






void printFileslist(struct CountryFiles* fileslist){

    if (fileslist == NULL){
        printf("List of Files is Empty!\n\n");
        return;
    }

    printf("\nList of Folders:\n");

    while(fileslist != NULL){
        printf("%s  ", fileslist->filename);
        fileslist = fileslist->next;
    }

    printf("\n");
}

