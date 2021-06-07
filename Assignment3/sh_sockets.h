#ifndef __PIPES__
#define __PIPES__

#include <dirent.h>
#include "virus.h"
#include "countries.h"

#include <netinet/in.h>

struct MonitorStruct{
    int sock;      //monitorServer's connection
    int port;
    int pid;    //monitorServer's process id
    struct sockaddr_in server;
    struct sockaddr *serverptr;
    struct VirusesListMain *viruses;    //list of viruses+bf for this monitor
    struct CountryFiles* list_of_countries; //list of countries for monitorServer[i] to handle
    int numOfCountries;                         // the number of those countries
};


struct Arguments{

    int socketBufferSize;             //-b
    int numMonitors;    //-m 
    char *input_dir;    //-i
    int cyclicBufferSize;
    int numofthreads;
    
};

void create_child(int i);
int send_message(int fd, const void* message, int size_of_message, int socketbuffersize);

char* get_message(int fd_r, int socketbuffersize);

int send_bloomfilters(int fd, struct List* virus_list, int socketbuffersize);
int get_bloomfilters(struct MonitorStruct *commun, int socketbuffersize, int numMonitors, int i, int replace);

#endif