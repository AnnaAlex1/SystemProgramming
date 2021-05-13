#ifndef __VIRUS__
#define __VIRUS__


#include "skiplist.h"
#include "bloomfilter.h"

struct List{
    char* name;

    struct BloomFilter* vacc_bloom;
    VacSkipList vaccinated;
    int levels_vac;                 //number of levels in vaccinated skip list

    struct BloomFilter* not_vacc_bloom;
    NotVacSkipList not_vacc;
    int levels_notvac;              //number of levels in not_vaccinated skip list

    struct List* next;
};


void addinlist(struct List** virus_list, char* name);
int checkifinlist(struct List* virus_list, char* name);
int deletelist(struct List** virus_list);
struct List* getelemfromlist(struct List* virus_list, char* element);
void printlist(struct List* virus_list);


///////////////////////////////////////////////////

struct VirusesListMain{
    char* name;
    struct BloomFilter* vacc_bloom;
    struct VirusesListMain* next;
};



void addinVirMain(struct VirusesListMain** virus_list, char* name, unsigned int *bloom);
int deleteVirMain(struct VirusesListMain** virus_list);
struct VirusesListMain* getelemfromVirMain(struct VirusesListMain* virus_list, char* element);
void printVirMain(struct VirusesListMain* virus_list);







#endif