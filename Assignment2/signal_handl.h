#ifndef __SIGNAL__
#define __SIGNAL__

void handle_recreate(int sig);
void handle_newfiles(int sig);
void handle_MonitorFin(int sig);
void handle_ParentFin(int sig);
void handle_needtoread(int sig);

#endif