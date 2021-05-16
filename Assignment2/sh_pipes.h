#ifndef __PIPES__
#define __PIPES__

#include <dirent.h>
#include "virus.h"


struct MonitorStruct{
    int fd_w;
    int fd_r;
    int pid;
    char *fifoname_w;
    char *fifoname_r;
    struct VirusesListMain *viruses;
};


void create_child(int i, int buffersize);
int send_message(int fd, const void* message, int size_of_message, size_t buffersize);

char* get_message(int fd_r, size_t buffersize);

int send_bloomfilters(int fd, struct List* virus_list, size_t buffersize);
int get_bloomfilters(struct MonitorStruct *commun, size_t buffersize, int numMonitors, int i, int replace);

#endif