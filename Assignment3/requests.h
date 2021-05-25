#ifndef __REQUESTS__
#define __REQUESTS__


struct RequestsList{

    char* virus;
    char* date;
    char *country;
    int approved;

    struct RequestsList *next;

};

void addRequest(struct RequestsList** req_list, char* virus, char* date, char* country, int approved);
void printReqlist(struct RequestsList* req_list);
int deleteReqlist(struct RequestsList** req_list);

#endif