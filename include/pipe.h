#ifndef __PIPE_H__
#define __PIPE_H__

#include <stdlib.h>
#include <unistd.h>
#include "readcmd.h"
#include "handler.h"
#include "commandes_internes.h"
#include "utils.h" 
#include "csapp.h"
#include "process.h"



void redirectIOPipe(int cmdCourant , int nbPipes, int **fd);
void pipeCommande(cmdline *l , pid_t *childpid , Process *process , Job *jobs);
void closeAllPipe(int **fd , int nbPipe);
void launchChild(cmdline *l, int i, int numPipe, int nbPipes, int **fd , pid_t *childpid , Job *jobs);


#endif