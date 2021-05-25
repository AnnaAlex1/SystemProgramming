#include "citizens.h"
#include "records.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>




Hashtable hashtable_create(){
    
    Hashtable ht;

    ht = malloc(sizeof(struct TablePos)*TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        ht[i].bucket = NULL;
    }
    

    return ht;
}




void hashtable_destroy(Hashtable ht){

    if (ht == NULL){
        return;
    }

    printf("Destroying the Hashtable...\n");


    struct Bucket* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        

        while (ht[i].bucket != NULL){

            current_buc = ht[i].bucket;

            for (int j = 0; j < BUC_SIZE; j++){
                free_citizen(current_buc->element[j]);
            }
            
            free(current_buc->element);

            ht[i].bucket = current_buc->next_buc;

            free(current_buc);

        }
           
    }
    
}





struct Bucket* bucket_create(){

    struct Bucket* new_bucket;

    new_bucket = malloc(sizeof(struct Bucket));
    
    new_bucket->element = malloc(sizeof(struct Citizen)*BUC_SIZE );
    new_bucket->cur_size=0;
    new_bucket->next_buc=NULL;

    for (int i = 0; i < BUC_SIZE; i++){
        new_bucket->element[i].citizenID=NULL;
        new_bucket->element[i].firstname=NULL;
        new_bucket->element[i].lastname=NULL;
        new_bucket->element[i].country=NULL;
    }
    

    return new_bucket;
}






void hashtable_add(Hashtable ht, struct Citizen cit){

    //printf("Citizen Hashtable: ADDING %s\n", cit->citizenID);
    
    struct Citizen* new_cit;
    new_cit = malloc(sizeof(struct Citizen));
    new_cit->citizenID = malloc(sizeof(char) * (strlen(cit.citizenID)+1));
    strcpy(new_cit->citizenID, cit.citizenID);
    new_cit->firstname = malloc(sizeof(char) * (strlen(cit.firstname)+1));
    strcpy(new_cit->firstname, cit.firstname);   
    new_cit->lastname = malloc(sizeof(char) * (strlen(cit.lastname)+1));
    strcpy(new_cit->lastname, cit.lastname);
    new_cit->country = malloc(sizeof(char) * (strlen(cit.country)+1));
    strcpy(new_cit->country, cit.country);
    new_cit->age = cit.age;
    

    int pos = hashfunction(cit.citizenID);

    struct Bucket* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){    //if row is empty
        ht[pos].bucket = bucket_create();
        bucket = ht[pos].bucket;
        memcpy( &bucket->element[bucket->cur_size], new_cit, sizeof(struct Citizen) );
        bucket->cur_size++; 
    } else {    //find an empty position

        bucket = ht[pos].bucket;
        while (bucket->next_buc != NULL && bucket->cur_size == BUC_SIZE){
            bucket = bucket->next_buc;
        }

        if (bucket->next_buc == NULL ){  //case: no empty bucket, need to create another
            
            bucket->next_buc = bucket_create();
            memcpy( &bucket->element[bucket->cur_size], new_cit, sizeof(struct Citizen) );
            bucket->cur_size++;   

        } else if (bucket->cur_size < BUC_SIZE){        //case: found a bucket with space
            
            
            for (int j=0; j<BUC_SIZE; j++){
                if (bucket->element[j].citizenID == NULL){
                memcpy( &bucket->element[bucket->cur_size], new_cit, sizeof(struct Citizen) );
                bucket->cur_size++; 
                    break;
                }
            }

        }
    }

    free(new_cit);


}



int hashtable_find(Hashtable ht, char* element){

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return 0;
    }

    int pos = hashfunction(element);        //find num of row

    struct Bucket* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){    //if empty -> not found
        return 0;
    } else {

        bucket = ht[pos].bucket;
        while (bucket != NULL){ //traverse row

            for (int i = 0; i < BUC_SIZE; i++)
            {
                if (bucket->element[i].citizenID != NULL){    //if position is not empty

                    if ( strcmp(bucket->element[i].citizenID, element ) == 0 ){    //if found
                        return 1;
                    }

                }
            }
            


            bucket = bucket->next_buc;
        }

    }

    return 0;
}



struct Citizen* hashtable_get(Hashtable ht, char* element){

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return NULL;
    }

    int pos = hashfunction(element);     //find num of row

    struct Bucket* bucket = ht[pos].bucket;

    if (ht[pos].bucket == NULL){       //if empty -> not found
        return NULL;
    } else {

        bucket = ht[pos].bucket;
        while (bucket != NULL){

            for (int i = 0; i < BUC_SIZE; i++)
            {
                if (bucket->element[i].citizenID != NULL){    //if position is not empty

                    if ( strcmp(bucket->element[i].citizenID, element ) == 0 ){    //if found
                        return &(bucket->element[i]);
                    }

                }
            }
            


            bucket = bucket->next_buc;
        }

    }
    return NULL;
}






int hashfunction(char* string){
    int hashvalue = 0;

    int size = strlen(string);

    for (int i = 0; i < size; i++)
    {
        hashvalue += string[i];
    }
    
    return hashvalue%TABLE_SIZE;

}


void print_hashtable(Hashtable ht){

    printf("\n");

    if (ht == NULL){
        printf("Hashtable is Empty!\n");
        return;
    }

    printf("My Hashtable:\n");
    struct Bucket* current_buc;

    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        current_buc = ht[i].bucket;

        while ( current_buc != NULL){

            for (int j = 0; j < BUC_SIZE; j++){
                if (current_buc->element[j].citizenID != NULL)
                    print_citizen(current_buc->element[j]);
            }
            
            
            current_buc = current_buc->next_buc;

            

        }
        //printf("\n");
           
    } 
    printf("\n");
}





void print_citizen(struct Citizen cit){

    //printf("\nCitizen:\n");
    printf("%s  %s   %s", cit.citizenID, cit.firstname, cit.lastname);
    printf("    %s   %d\n", cit.country, cit.age);
}


void free_citizen(struct Citizen cit){
    if (cit.citizenID != NULL) free(cit.citizenID);
    if (cit.firstname != NULL) free(cit.firstname);
    if (cit.lastname != NULL) free(cit.lastname);
    if (cit.country != NULL) free(cit.country);
}