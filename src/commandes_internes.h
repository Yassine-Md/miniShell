#ifndef __COMMANDES_INTERNES_H__
#define __COMMANDES_INTERNES_H__

#include "readcmd.h"

bool isInterne(char *cmd);
void errorFork(pid_t pid);
int nbCmd(cmdline *l);
void exeCmdInterne(cmdline *l, int i);
void execCmdExterne(struct cmdline *l, int i);

#endif