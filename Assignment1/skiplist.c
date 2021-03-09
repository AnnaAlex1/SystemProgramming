#include "skiplist.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int num_of_levels = 1;
int max_level = 4;  ///?????


/*
int main(void){

    srand( time(0) );

    SkipList list = NULL;
    list = malloc(sizeof(struct PtrNodes));
    list->down = NULL;
    list->next = NULL;
    list->elem = NULL;


    struct SkipRecord rec;
    rec.name = malloc(sizeof(char*)*( strlen("name1")+1 ));
    
    strcpy(rec.name, "name1");
    addSkipList(&list, &rec);
    
    printSkipList(list);

    struct SkipRecord rec2;
    rec2.name = malloc(sizeof(char*)*( strlen("name1")+1 ));
    strcpy(rec2.name, "name2");
    addSkipList(&list, &rec2);

    printSkipList(list);

    struct SkipRecord rec3;
    rec3.name = malloc(sizeof(char*)*( strlen("name1")+1 ));
    strcpy(rec3.name, "name3");
    addSkipList(&list, &rec3);


    struct SkipRecord rec6;
    rec6.name = malloc(sizeof(char*)*( strlen("name10")+1 ));
    strcpy(rec6.name, "name10");
    addSkipList(&list, &rec6);

    printSkipList(list);


    struct SkipRecord rec4;
    rec4.name = malloc(sizeof(char*)*( strlen("name1")+1 ));
    strcpy(rec4.name, "name4");
    addSkipList(&list, &rec4);

    struct SkipRecord rec5;
    rec5.name = malloc(sizeof(char*)*( strlen("name1")+1 ));
    strcpy(rec5.name, "name5");
    addSkipList(&list, &rec5);


    printSkipList(list);


    printf("Deleting element: %s\n", rec6.name);
    if ( removeSkipList(&list, &rec6) == 1){
        printf("Successful deleting of element\n");
    }


    printSkipList(list);

    printf("Deleting element: %s\n", rec5.name);
    if ( removeSkipList(&list, &rec5) == 1){
        printf("Successful deleting of element\n");
    }

    printSkipList(list);


    printf("\n\n\n");
    if ( searchSkipList(list, "name6") == 1){
        printf("Item 'name6' found!!!\n");
    } else {
        printf("Not found!!!\n");
    }



    printf("\nFreeing List\n");
    freeSkipList(&list);

}
*/


//FOR VACCINTATED SKIPLIST

int searchVacSkipList(VacSkipList list, char* id){

    if (list == NULL || list->next == NULL){
        return -1;
    }

    struct VacPtrNodes* temp;

    temp = list;

    while ( temp != NULL ){

        while( temp!= NULL && temp->next != NULL && strcmp(temp->next->elem->name,id) < 0 ){
            temp = temp->next;
        }

        if (temp == NULL){
            printf("Error\n");
            return -1;
        } else if (temp->next != NULL && strcmp(temp->next->elem->name,id) == 0){     //element found
            return 1;
        } else if (temp->next == NULL && strcmp(temp->elem->name,id) == 0){
            return 1;
        }

        temp = temp->down;


    }

    return 0;

}





char* getDate_VacSkipList(VacSkipList list, char* id){

    if (list == NULL || list->next == NULL){
        return NULL;
    }

    struct VacPtrNodes* temp;

    temp = list;

    while ( temp != NULL ){

        while( temp!= NULL && temp->next != NULL && strcmp(temp->next->elem->name,id) < 0 ){
            temp = temp->next;
        }

        if (temp == NULL){
            printf("Error\n");
            return NULL;
        } else if (temp->next != NULL && strcmp(temp->next->elem->name,id) == 0){     //element found
            return temp->next->elem->date;
        } else if (temp->next == NULL && strcmp(temp->elem->name,id) == 0){
            return temp->next->elem->date;
        }

        temp = temp->down;


    }

    return NULL;

}





int addVacSkipList(VacSkipList *list, struct VacSkipRecord* element){
    
    printf("\n");

    printf("VaccList: ADDING: %s\n", element->name);
    //printf("Current height = %d\n", num_of_levels);

    if((*list) == NULL ){
        return -1;
    }


    if ((*list)->next == NULL){       //list is empty
        (*list)->next = malloc(sizeof(struct VacPtrNodes));
        (*list)->next->elem = element;
        (*list)->next->next = NULL;
        (*list)->next->down = NULL;
        return 1;
    }


    struct VacPtrNodes* temp, *prev, *new_node, *path_prev;

    struct VacPath* path = NULL, *path_temp;                                   //ARRAYYYYY

    struct VacPtrNodes* new_head;

    int height = 1;

    int coin;




    temp = (*list)->next;
    prev = *list;

    while ( prev != NULL ){

        
        if ( prev->down != NULL){   //level > 0

            printf("Searching for the first element with greater value\n");
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                
                prev = temp;
                temp = temp->next;
                printf("New Prev = %s, \n", prev->elem->name);
            }

            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Already in!\n");
                return -1;
            }
            printf("Limit found. Adding to path...\n");

            //adding to path
            path_temp = path;
            path = malloc(sizeof(struct VacPath));
            path->ptr = prev;
            path->next = path_temp;


            printf("Going down...\n");
            prev = prev->down;
            temp = prev->next;   
            
        } else {  // level = 0

            printf("Lowest level. Allocating new space...\n");
            
            //find right location
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                prev = temp;
                temp = temp->next;
            }
            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Already in!\n");
                return -1;
            }
            prev->next = malloc(sizeof(struct VacPtrNodes));
            prev->next->elem = element;
            prev->next->next = temp;
            prev->next->down = NULL;


            //deciding height
            height = 1;

            path_prev = prev->next;     //for "down" pointer
            path_temp = path;           //hold the beginning

            coin = rand()%2;

            while ( coin == 1 && height < max_level){     //flip coin
                
                printf("Increasing height of node\n");
                
                new_node = malloc(sizeof(struct VacPtrNodes));
                new_node->elem = element;
                new_node->down = path_prev;
                

                height++;

                //if heigth > num_of_levels, add head!!!
                if (height > num_of_levels){
                    new_head = malloc(sizeof(struct VacPtrNodes));
                    new_head->down = *list;
                    new_head->next = new_node;
                    new_head->elem = NULL;
                    new_node->next = NULL;
                    *list = new_head;
                    num_of_levels++;

                } else {

                    if (path != NULL){  //if there is a path
                        
                        new_node->next = path->ptr->next;
                        path->ptr->next = new_node;

                        path = path->next;

                    } 

                }
                
                path_prev = new_node;              

                coin = rand()%2;
                printf("Coin: %d\n", coin);

            }
            

            //free path
            
            while (path_temp != NULL){
                path = path_temp;
                path_temp = path_temp->next;
                free(path);
            }
            path = NULL;

            


            return 1;
                
        }


    }

    return 0;

}







int removeVacSkipList(VacSkipList *list, struct VacSkipRecord* element){

    printf("\n\n");

    printf("Deleting element: %s\n", element->name);

    if ( *list == NULL || (*list)->next == NULL){
        printf("List is empty. Cannot delete element.\n");
        return -1;
    }


    struct VacPtrNodes* temp, *prev, *to_free;



    temp = (*list)->next;
    prev = *list;

    while ( prev != NULL ){

        if ( prev->elem != NULL){
            printf("Starting row with prev = %s\n", prev->elem->name);
        } else if (temp != NULL){
            printf("Starting row with temp = %s\n", temp->elem->name);
        }
        
        if ( prev->down != NULL){   //level > 0

            printf("Searching for the first element with greater value\n");
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                
                prev = temp;
                temp = temp->next;
                printf("New Prev = %s, \n", prev->elem->name);
            }

            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Found one!\n");
                to_free = temp;
                prev->next = temp->next;
                free(to_free);
            }

            printf("Going down...\n");
            prev = prev->down;
            temp = prev->next; 

            printf("Deleting head of this level...\n");
            if ( (*list)->next == NULL && (*list)->down != NULL){
                to_free = (*list);
                (*list) = (*list)->down;
                free(to_free);
                num_of_levels--;
            }  
            
        } else {  // level = 0

            printf("Lowest level.\n");
            
            //find element
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                prev = temp;
                temp = temp->next;
            }

            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Found the element!\n");
                to_free = temp;
                prev->next = temp->next;
                free(to_free->elem->name);
                free(to_free->elem->date);
                //free(to_free->elem);
                free(to_free);
                return 1;
            }      

    
        }


    }

    return -1;

}






void printVacSkipList(VacSkipList list){

    printf("\n");

    if (list == NULL){
        printf("No list. Cannot print.\n");
        return;
    }

    if (list->next == NULL){
        printf("List is empty!\n");
    }


    struct VacPtrNodes* temp;


    while( list != NULL){

        temp = list->next;

        while ( temp != NULL && temp->elem != NULL){
            printf("%s %s,    ", temp->elem->name, temp->elem->date);
            temp = temp->next;
        }

        printf("\n");
        list = list->down;
    }



    
}



void freeVacSkipList(VacSkipList *list){

    if (*list == NULL){
        printf("No list. Cannot delete.\n");
        return;
    }

    if ( (*list)->next == NULL){       //list is empty, just delete head
        free( (*list) );
        *list = NULL;
    }

    struct VacPtrNodes* temp_head;
    struct VacPtrNodes* temp;


    while( *list != NULL ){

        while ( (*list)->next != NULL){
            
            //temp = (*list)->next;
            if (num_of_levels == 1){      //if on last level
                free((*list)->next->elem->name);
                free((*list)->next->elem->date);
                //free((*list)->next->elem);

            }

            temp = (*list)->next->next;
            free((*list)->next);
            (*list)->next = temp;

        }

        //free head
        temp_head = *list;
        (*list) = (*list)->down;
        free(temp_head);
        num_of_levels--;
    }


}




//////////////////////////////////////////////////////////////////////////
//FOR NOT VACCINTATED SKIPLIST

int searchNotVacSkipList(NotVacSkipList list, char* id){

    if (list == NULL || list->next == NULL){
        return -1;
    }

    struct NotVacPtrNodes* temp;

    temp = list;

    while ( temp != NULL ){

        while( temp!= NULL && temp->next != NULL && strcmp(temp->next->elem->name,id) < 0 ){
            temp = temp->next;
        }

        if (temp == NULL){
            printf("Error\n");
            return -1;
        } else if (temp->next != NULL && strcmp(temp->next->elem->name,id) == 0){     //element found
            return 1;
        } else if (temp->next == NULL && strcmp(temp->elem->name,id) == 0){
            return 1;
        }

        temp = temp->down;


    }

    return 0;

}





int addNotVacSkipList(NotVacSkipList *list, struct NotVacSkipRecord* element){
    
    printf("\n");

    printf("NotVaccList: ADDING: %s\n", element->name);
    //printf("Current height = %d\n", num_of_levels);

    if((*list) == NULL ){
        return -1;
    }

    if ((*list)->next == NULL){       //list is empty
        (*list)->next = malloc(sizeof(struct NotVacPtrNodes));
        (*list)->next->elem = element;
        (*list)->next->next = NULL;
        (*list)->next->down = NULL;
        return 1;
    }


    struct NotVacPtrNodes* temp, *prev, *new_node, *path_prev;

    struct NotVacPath* path = NULL, *path_temp;                                   //ARRAYYYYY

    struct NotVacPtrNodes* new_head;

    int height = 1;

    int coin;




    temp = (*list)->next;
    prev = *list;

    while ( prev != NULL ){

        
        if ( prev->down != NULL){   //level > 0

            printf("Searching for the first element with greater value\n");
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                
                prev = temp;
                temp = temp->next;
                printf("New Prev = %s, \n", prev->elem->name);
            }

            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Already in!\n");
                return -1;
            }
            printf("Limit found. Adding to path...\n");

            //adding to path
            path_temp = path;
            path = malloc(sizeof(struct NotVacPath));
            path->ptr = prev;
            path->next = path_temp;


            printf("Going down...\n");
            prev = prev->down;
            temp = prev->next;   
            
        } else {  // level = 0

            printf("Lowest level. Allocating new space...\n");
            
            //find right location
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                prev = temp;
                temp = temp->next;
            }
            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Already in!\n");
                return -1;
            }
            prev->next = malloc(sizeof(struct NotVacPtrNodes));
            prev->next->elem = element;
            prev->next->next = temp;
            prev->next->down = NULL;


            //deciding height
            height = 1;

            path_prev = prev->next;     //for "down" pointer
            path_temp = path;           //hold the beginning

            coin = rand()%2;

            while ( coin == 1 && height < max_level){     //flip coin
                
                printf("Increasing height of node\n");
                
                new_node = malloc(sizeof(struct NotVacPtrNodes));
                new_node->elem = element;
                new_node->down = path_prev;
                

                height++;

                //if heigth > num_of_levels, add head!!!
                if (height > num_of_levels){
                    new_head = malloc(sizeof(struct NotVacPtrNodes));
                    new_head->down = *list;
                    new_head->next = new_node;
                    new_head->elem = NULL;
                    new_node->next = NULL;
                    *list = new_head;
                    num_of_levels++;

                } else {

                    if (path != NULL){  //if there is a path
                        
                        new_node->next = path->ptr->next;
                        path->ptr->next = new_node;

                        path = path->next;

                    } 

                }
                
                path_prev = new_node;              

                coin = rand()%2;
                printf("Coin: %d\n", coin);

            }
            

            //free path
            
            while (path_temp != NULL){
                path = path_temp;
                path_temp = path_temp->next;
                free(path);
            }
            path = NULL;

            


            return 1;
                
        }


    }

    return 0;

}







int removeNotVacSkipList(NotVacSkipList *list, struct NotVacSkipRecord* element){

    printf("\n\n");

    printf("Deleting element: %s\n", element->name);

    if ( *list == NULL || (*list)->next == NULL){
        printf("List is empty. Cannot delete element.\n");
        return -1;
    }


    struct NotVacPtrNodes* temp, *prev, *to_free;



    temp = (*list)->next;
    prev = *list;

    while ( prev != NULL ){

        if ( prev->elem != NULL){
            printf("Starting row with prev = %s\n", prev->elem->name);
        } else if (temp != NULL){
            printf("Starting row with temp = %s\n", temp->elem->name);
        }
        
        if ( prev->down != NULL){   //level > 0

            printf("Searching for the first element with greater value\n");
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                
                prev = temp;
                temp = temp->next;
                printf("New Prev = %s, \n", prev->elem->name);
            }

            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Found one!\n");
                to_free = temp;
                prev->next = temp->next;
                free(to_free);
            }

            printf("Going down...\n");
            prev = prev->down;
            temp = prev->next; 

            printf("Deleting head of this level...\n");
            if ( (*list)->next == NULL && (*list)->down != NULL){
                to_free = (*list);
                (*list) = (*list)->down;
                free(to_free);
                num_of_levels--;
            }  
            
        } else {  // level = 0

            printf("Lowest level.\n");
            
            //find element
            while( temp!= NULL && strcmp(temp->elem->name,element->name) < 0 ){
                prev = temp;
                temp = temp->next;
            }

            if ( temp!= NULL && strcmp(temp->elem->name,element->name) == 0 ){
                printf("Found the element!\n");
                to_free = temp;
                prev->next = temp->next;
                free(to_free->elem->name);
                //free(to_free->elem);
                free(to_free);
                return 1;
            }      

    
        }


    }

    return -1;

}






void printNotVacSkipList(NotVacSkipList list){

    printf("\n");

    if (list == NULL){
        printf("No list. Cannot print.\n");
        return;
    }

    if (list->next == NULL){
        printf("List is empty!\n");
    }


    struct NotVacPtrNodes* temp;


    while( list != NULL){

        temp = list->next;

        while ( temp != NULL && temp->elem != NULL){
            printf("%s   ", temp->elem->name);
            temp = temp->next;
        }

        printf("\n");
        list = list->down;
    }



    
}



void freeNotVacSkipList(NotVacSkipList *list){

    if (*list == NULL){
        printf("No list. Cannot delete.\n");
        return;
    }

    if ( (*list)->next == NULL){       //list is empty, just delete head
        free( (*list) );
        *list = NULL;
    }

    struct NotVacPtrNodes* temp_head;
    struct NotVacPtrNodes* temp;


    while( *list != NULL ){

        while ( (*list)->next != NULL){
            
            //temp = (*list)->next;
            if (num_of_levels == 1){      //if on last level
                free((*list)->next->elem->name);
                //free((*list)->next->elem);

            }

            temp = (*list)->next->next;
            free((*list)->next);
            (*list)->next = temp;

        }

        //free head
        temp_head = *list;
        (*list) = (*list)->down;
        free(temp_head);
        num_of_levels--;
    }


}

