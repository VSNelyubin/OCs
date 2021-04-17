#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <signal.h>

extern void sign_handler(int sig, struct sigcontext *sc);
extern void alarm_handler(int sig, struct sigcontext *sc);
#endif
