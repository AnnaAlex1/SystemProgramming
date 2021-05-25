#include "requests.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void addRequest(struct RequestsList** req_list, char* virus, char* date, char* country, int approved){


    struct RequestsList* newnode;
    newnode = malloc(sizeof(struct RequestsList));
    newnode->virus = malloc(sizeof(char)* (strlen(virus)+1));
    memcpy(newnode->virus, virus, sizeof(char)* (strlen(virus)+1) );

    newnode->date = malloc(sizeof(char)* (strlen(date)+1));
    memcpy(newnode->date, date, sizeof(char)* (strlen(date)+1) );

    newnode->country = malloc(sizeof(char)* (strlen(country)+1));
    memcpy(newnode->country, country, sizeof(char)* (strlen(country)+1) );

    newnode->approved=approved;

    newnode->next = NULL;




    struct RequestsList* temp;

    if (*req_list == NULL){
        *req_list = newnode;
    } else {
        temp = *req_list;

        //place at the beginning:
        *req_list = newnode;
        newnode->next = temp;
        
    }

    return;

}




void printReqlist(struct RequestsList* req_list){

    if (req_list == NULL){
        printf("List is Empty!\n\n");
        return;
    }

    printf("\nMy List:\n");

    while(req_list != NULL){
        printf("%s  %s %s\n", req_list->virus, req_list->date, req_list->country);
        req_list = req_list->next;
    }

  
}







int deleteReqlist(struct RequestsList** req_list){

    printf("Deleting Requests...\n");
    if (*req_list == NULL) return -1;

    struct RequestsList* to_free;

    while(*req_list != NULL){
        to_free = *req_list;
        *req_list = (*req_list)->next;
        free(to_free->virus);
        free(to_free->date);
        free(to_free->country);
        free(to_free);
    }

    return 0;

}

