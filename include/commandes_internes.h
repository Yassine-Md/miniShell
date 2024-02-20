#ifndef __COMMANDES_INTERNES_H__
#define __COMMANDES_INTERNES_H__

#include "readcmd.h"
#include <stdbool.h>
#define nbCmdInterne 3

bool isInterne(char *cmd);
void errorFork(pid_t pid);
int nbCmd(cmdline *l);
void exeCmdInterne(cmdline *l, int i);
void execCmdExterne(struct cmdline *l, int i);
void execCmdWithPipe(struct cmdline *l , int i);
void redirectIOPipe(int cmdCourant , int nbCmd, int **fd);
#endif

//cat fichier.txt | sort | grep pomme