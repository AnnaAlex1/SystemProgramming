#ifndef __SKIP__
#define __SKIP__


//FOR VACCINATED SKIPLIST
struct VacSkipRecord{  //the info we want to keep at the list
    char* name;
    char* date;
};


struct VacPtrNodes{     //nodes to represent the element in each level
    struct VacSkipRecord* elem;     //pointer to the node with the element it represents
    struct VacPtrNodes* down;
    struct VacPtrNodes* next;
};


struct VacPath{ //list to maintain the path
    struct VacPtrNodes* ptr;
    struct VacPath* next;
};


typedef struct VacPtrNodes* VacSkipList;


int addVacSkipList(VacSkipList *list, struct VacSkipRecord* element, int *num_of_levels);
int removeVacSkipList(VacSkipList *list, char* id, int *num_of_levels);
int searchVacSkipList(VacSkipList list, char* id);
char* getDate_VacSkipList(VacSkipList list, char* id);
void printVacSkipList(VacSkipList list);
void freeVacSkipList(VacSkipList *list, int *num_of_levels);




//FOR NOT VACCINATED SKIPLIST

struct NotVacSkipRecord{  //the info we want to keep at the list
    char* name;
};


struct NotVacPtrNodes{     //nodes to represent the element in each level
    struct NotVacSkipRecord* elem;     //pointer to the node with the element it represents
    struct NotVacPtrNodes* down;
    struct NotVacPtrNodes* next;
};


struct NotVacPath{  //list to maintain the path
    struct NotVacPtrNodes* ptr;
    struct NotVacPath* next;
};


typedef struct NotVacPtrNodes* NotVacSkipList;


int addNotVacSkipList(NotVacSkipList *list, struct NotVacSkipRecord* element, int *num_of_levels);
int removeNotVacSkipList(NotVacSkipList *list, char* id, int *num_of_levels);
int searchNotVacSkipList(NotVacSkipList list, char* id);
void printNotVacSkipList(NotVacSkipList list);
void freeNotVacSkipList(NotVacSkipList *list, int *num_of_levels);





#endif
