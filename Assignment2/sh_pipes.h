#ifndef __PIPES__
#define __PIPES__

#include <dirent.h>
#include "virus.h"


struct MonitorStruct{
    int fd_w;   //file descriptor for writing
    int fd_r;   //file descriptor for reading
    int pid;    //monitor's process id
    char *fifoname_w;       //named pipe's filename (for writing)
    char *fifoname_r;       //named pipe's filename (for reading)
    struct VirusesListMain *viruses;    //list of viruses+bf for this monitor
};


void create_child(int i, int buffersize);
int send_message(int fd, const void* message, int size_of_message, size_t buffersize);

char* get_message(int fd_r, size_t buffersize);

int send_bloomfilters(int fd, struct List* virus_list, size_t buffersize);
int get_bloomfilters(struct MonitorStruct *commun, size_t buffersize, int numMonitors, int i, int replace);

#endif