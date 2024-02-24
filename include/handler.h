#ifndef __HANDLER_H__
#define __HANDLER_H__

#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include "job.h"
#include <unistd.h>

void SigChildHandler(int sig);
void handlerSigInt(int sig);
void handlerCtrlZ(int sig);


#endif
