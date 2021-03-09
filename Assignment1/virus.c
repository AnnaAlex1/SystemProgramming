#include "virus.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



void addinlist(struct List** virus_list, char* name){

    //printf("Virus List: ADDING %s\n", name);

    struct List* newnode;
    newnode = malloc(sizeof(struct List));
    newnode->name = malloc(sizeof(char)* (strlen(name)+1));
    memcpy(newnode->name, name, sizeof(char)* (strlen(name)+1) );
    newnode->vacc_bloom = init_Bloom();
    
    //vaccinated list
    newnode->vaccinated = malloc(sizeof(struct VacPtrNodes));
    newnode->vaccinated->down = NULL;
    newnode->vaccinated->next = NULL;
    newnode->vaccinated->elem = NULL;
    newnode->vacc_num = 0;

    newnode->not_vacc_bloom = init_Bloom();

    //not-vaccinated list
    newnode->not_vacc = malloc(sizeof(struct NotVacPtrNodes));
    newnode->not_vacc->down = NULL;
    newnode->not_vacc->next = NULL;
    newnode->not_vacc->elem = NULL;
    newnode->not_vac_num = 0;

    newnode->next = NULL;




    struct List* temp;

    if (*virus_list == NULL){
        *virus_list = newnode;
    } else {
        temp = *virus_list;

        //place at the beginning:
        *virus_list = newnode;
        newnode->next = temp;
    }

    return;

}



//returns -1 if not found, 0 if found
int checkifinlist(struct List* virus_list, char* name){

    if (virus_list == NULL) return -1;

    while (virus_list != NULL && strcmp(virus_list->name, name) != 0){

        virus_list = virus_list->next;
    }

    if (virus_list == NULL){        //case: not found
        return -1;
    } 

    return 0;

}





//returns element if found, otherwise returns NULL
struct List* getelemfromlist(struct List* virus_list, char* element){

    if (virus_list == NULL) return NULL;

    while (virus_list != NULL && strcmp(virus_list->name,element) != 0){

        virus_list = virus_list->next;
    }

    if (virus_list == NULL){        //case: not found
        return NULL;
    } 

    return virus_list;

}


void printlist(struct List* virus_list){

    if (virus_list == NULL){
        printf("List is Empty!\n\n");
        return;
    }

    printf("\nMy List:\n");

    while(virus_list != NULL){
        printf("%s  ", virus_list->name);
        virus_list = virus_list->next;
    }

    printf("\n");
}



int deletelist(struct List** virus_list){

    printf("Deleting List...\n");
    if (*virus_list == NULL) return -1;

    struct List* to_free;

    while(*virus_list != NULL){
        to_free = *virus_list;
        *virus_list = (*virus_list)->next;
        free(to_free->name);
        destroy_Bloom(to_free->not_vacc_bloom);
        destroy_Bloom(to_free->vacc_bloom);
        freeVacSkipList(&(to_free->vaccinated));
        freeNotVacSkipList(&(to_free->not_vacc));
        free(to_free);
    }

    return 0;

}