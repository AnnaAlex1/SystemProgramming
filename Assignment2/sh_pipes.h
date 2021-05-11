#ifndef __PIPES__
#define __PIPES__

#include <dirent.h>

int sent_message_wrong(int fd, const void* message, size_t bufferSize);
//int sent_message(int fd, const void* message);

//char* get_message(int fd_r, size_t buffersize);
char* get_message_wrong(int fd_r, size_t buffersize);


#endif