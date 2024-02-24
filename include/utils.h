#ifndef __UTILS_H__
#define __UTILS_H__

#include "readcmd.h"

//void createPipes(int fd[][2], int n);
void affichage(cmdline *l);
void freeDescripteurs(int** fd, int nbPipes);
int** allocateDescripteurs(int nbPipes);

#endif
