#ifndef __CITIZEN__
#define __CITIZEN__


#define TABLE_SIZE 3
#define BUC_SIZE 2


struct Citizen{
    char *citizenID;
    char *firstname; 
    char *lastname;
    char *country;
    int age;
};



struct TablePos{
    struct Bucket* bucket;
};


struct Bucket{
    struct Citizen* element;
    int cur_size;
    struct Bucket* next_buc;
};

typedef struct TablePos* Hashtable;



Hashtable hashtable_create();
struct Bucket* bucket_create();
void hashtable_add(Hashtable ht, struct Citizen cit);
int hashtable_find(Hashtable ht, char* citizenid);
struct Citizen* hashtable_get(Hashtable ht, char* element);
void hashtable_destroy(Hashtable ht);
void print_hashtable(Hashtable ht);

int hashfunction(char* string);


void print_citizen(struct Citizen cit);
void free_citizen(struct Citizen cit);




#endif