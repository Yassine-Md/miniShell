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
//void redirectIOPipe(int cmdCourant , int nbCmd, int **fd);
//void SigChildHandler(int sig); /*Si vous ignorez le signal SIGCHLD, cela signifie que le processus parent ne sera pas informé de la terminaison des processus fils. Cependant, cela ne prévient pas la création de zombies. Ignorer SIGCHLD permet simplement au système d'exploitation de nettoyer automatiquement les processus fils terminés sans que le processus parent ait besoin d'attendre explicitement leur terminaison.*/
void redirection_in_out (struct cmdline *l);
#endif

//cat fichier.txt | sort | grep pomme