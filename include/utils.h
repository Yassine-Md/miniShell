#ifndef __UTILS_H__
#define __UTILS_H__

#include "readcmd.h"

void createPipes(int fd[][2], int n);
void closeUnusedPipes(int fd[][2], int n, int currentPipe);
void closeAllPipe(int fd[][2], int n);
void affichage(cmdline *l);

#endif
