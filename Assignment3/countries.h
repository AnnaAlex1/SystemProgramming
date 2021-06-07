#ifndef __COUNTRY__
#define __COUNTRY__


#include <stdio.h>

struct Country{
    char *name;
    int num_of_files_read; 

};


struct TablePosCoun{
    struct BucketCoun* bucket;
};


struct BucketCoun{
    struct Country* element;
    int cur_size;
    struct BucketCoun* next_buc;
};

typedef struct TablePosCoun* CountryHash;



CountryHash hashtable_createCoun();
struct BucketCoun* bucket_createCoun();
void hashtable_addCoun(CountryHash ht, char* coun_name);
int hashtable_findCoun(CountryHash ht, char* country_name);
struct Country* hashtable_getCoun(CountryHash ht, char* country_name);
void set_num_of_files(CountryHash ht, char* country_name, int num_of_files);
void hashtable_destroyCoun(CountryHash ht);
void print_hashtableCoun(CountryHash ht, FILE* stream);


///////////////////////////////////


struct CountryMain{
    char *name;
    int pid;        //pid of monitor handling this country
};


struct TablePosCounMain{
    struct BucketCounMain* bucket;
};


struct BucketCounMain{
    struct CountryMain* element;
    int cur_size;
    struct BucketCounMain* next_buc;
};

typedef struct TablePosCounMain* CountryMainHash;



CountryMainHash hashtable_createCounMain();
struct BucketCounMain* bucket_createCounMain();
void hashtable_addCounMain(CountryMainHash ht, char* coun_name, int pid);
int hashtable_findCounMain(CountryMainHash ht, char* country_name);
struct CountryMain* hashtable_getCounMain(CountryMainHash ht, char* country_name);
void hashtable_destroyCounMain(CountryMainHash ht);
void print_hashtableCounMain(CountryMainHash ht, FILE* stream);




////////////////////////////////////////

struct CountryFiles{        //list to keep all folders to be used for monitor

    char *filename;
    struct CountryFiles *next;

};


void addinFilesList(struct CountryFiles** fileslist, char* filename);
int deleteFileslist(struct CountryFiles** fileslist);
void printFileslist(struct CountryFiles* fileslist);


#endif