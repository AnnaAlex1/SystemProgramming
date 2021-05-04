#ifndef __COUNTRY__
#define __COUNTRY__


struct Country{
    char *name;
    int population; 

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
void hashtable_destroyCoun(CountryHash ht);
void print_hashtableCoun(CountryHash ht);
void print_country(struct Country coun);

#endif