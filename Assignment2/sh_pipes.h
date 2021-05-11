#ifndef __PIPES__
#define __PIPES__

#include <dirent.h>
#include "virus.h"


struct MonitorStruct{
    int fd_w;
    int fd_r;
    int pid;
    struct VirusesListMain *viruses;
};



int sent_message_wrong(int fd, const void* message, size_t bufferSize);
//int sent_message(int fd, const void* message);

//char* get_message(int fd_r, size_t buffersize);
char* get_message_wrong(int fd_r, size_t buffersize);

int sent_bloomfilters(int fd, struct List* virus_list, size_t buffersize);
int get_bloomfilters(struct MonitorStruct *commun, size_t buffersize, int numMonitors);

#endif