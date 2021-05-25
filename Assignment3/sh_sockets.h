#ifndef __PIPES__
#define __PIPES__

#include <dirent.h>
#include "virus.h"
#include "countries.h"


struct MonitorStruct{
    int sock;      //monitor's connection
    int port;
    int pid;    //monitor's process id
    struct VirusesListMain *viruses;    //list of viruses+bf for this monitor
    struct CountryFiles* list_of_countries;
};


struct Arguments{

    size_t socketBufferSize;             //-b
    int numMonitors;    //-m 
    char *input_dir;    //-i
    int cyclicBufferSize;
    int numofthreads;
    
};

void create_child(int i);
int send_message(int fd, const void* message, int size_of_message, size_t buffersize);

char* get_message(int fd_r, size_t buffersize);

int send_bloomfilters(int fd, struct List* virus_list, size_t buffersize);
int get_bloomfilters(struct MonitorStruct *commun, size_t buffersize, int numMonitors, int i, int replace);

#endif