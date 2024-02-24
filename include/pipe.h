#ifndef __PIPE_H__
#define __PIPE_H__

#include <stdlib.h>
#include <unistd.h>
#include "readcmd.h"
#include "handler.h"
#include "commandes_internes.h"
#include "utils.h"
#include "csapp.h"



void redirectIOPipe(int cmdCourant , int nbPipes, int **fd);
void pipeCommande(cmdline *l);
void closeAllPipe(int **fd , int nbPipe);


#endif