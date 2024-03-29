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
        new_Bucket->element[i].population = 0;
    }
    

    return new_Bucket;
}






void hashtable_addCoun(CountryHash ht, char* coun_name){
    
    struct Country* new_coun;
    new_coun = malloc(sizeof(struct Country));
    new_coun->name = malloc(sizeof(char) * (strlen(coun_name)+1));
    strcpy(new_coun->name, coun_name);
    new_coun->population = 1;
    

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






void print_hashtableCoun(CountryHash ht){

    printf("\n");

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return;
    }

    printf("My Hashtable:\n");
    struct BucketCoun* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = ht[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                if (current_buc->element[j].name != NULL)
                    print_country(current_buc->element[j]);
            }
            
            
            current_buc = current_buc->next_buc;

            

        }
        //printf("\n");
           
    } 
    printf("\n");
}





void print_country(struct Country coun){

    printf("%s  %d\n", coun.name, coun.population);
}

