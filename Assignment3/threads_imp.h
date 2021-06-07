#ifndef __THR__
#define __THR__


#include <pthread.h>
#include <unistd.h>

#include "citizens.h"
#include "countries.h"
#include "virus.h"


struct JobBuffer {

    char **array;
    int start;
    int end;

};


struct thread_args{
    Hashtable citizens; 
    struct List** virus_list; 
    CountryHash countries;
    int cyclebuffersize;
};

int thread_store_records(pthread_t *my_thr, int numofthreads, int cyclebuffersize, int numofinput,
                     Hashtable citizens, struct List** virus_list, CountryHash countries);

struct JobBuffer *buffer_init(int cyclebuffersize);
int enqueue( struct JobBuffer *b, char *job, int cyclebuffersize);
char *dequeue( struct JobBuffer *b, int cyclebuffersize);
void buffer_destroy(struct JobBuffer **jb, int cyclebuffersize);

void *thread_fun(void *argp);

int fill_buffer(int numofinput, int cyclebuffersize, CountryHash countries);
int fill_buffer_new_files(Hashtable citizens, struct List** virus_list, CountryHash countries, int cyclicbuffersize);

void thread_finish(int numofthreads, int cyclebuffersize, pthread_t * my_thr);

#endif