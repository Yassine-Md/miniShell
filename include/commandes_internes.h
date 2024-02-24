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
void redirection_in_out (struct cmdline *l);
#endif

