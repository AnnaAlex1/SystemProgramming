#ifndef __CONSOLE__
#define __CONSOLE__


#include "virus.h"
#include "citizens.h"
#include "countries.h"
#include "sh_pipes.h"
#include "requests.h"

void console( struct MonitorStruct *commun, CountryMainHash countries, size_t bufferSize);

void travel_stat_all(struct RequestsList *req, CountryMainHash countries, char *virus, char* date1, char* date2);
void travel_stat_one(struct RequestsList *req, char *virus, char* date1, char* date2, char* country);
void travel_Req(struct MonitorStruct *commun, CountryMainHash countries, size_t bufferSize,
                char *virusName, char *countryFrom, char *citizenID, char *date, char* countryTo,
                 int *rejected_req, int *accepted_req, struct RequestsList **req_list);

int datecmp(char* date1, char* date2);
int in_prev_six(char *date1, char *date2);
char *get_cur_date();

#endif