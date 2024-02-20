/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "readcmd.h"
#include "utils.h"
#include "commandes_internes.h"
#include "csapp.h"


#define STDIN 0
#define STDOUT 1
#define STDERR 2

/*
comment savoir un processus a changer de status avec waitpid

la reception de sigchild est occurer lorsque le processus fils change d'etat 
*/


int** allocateDescripteurs(int nbPipes) {
    int** fd = malloc(sizeof(int*) * nbPipes);
    if (fd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Allouer deux cases pour chaque i
    for (int i = 0; i < nbPipes; i++) {
        fd[i] = malloc(sizeof(int) * 2);
        if (fd[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    return fd;
}

void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
	int nbPipes = n-1 ;
	int numPipe = 0;
    //int fd[n-1][2];

	int** fd = allocateDescripteurs(nbPipes);

	pid_t childpid;

    for (int i = 0; l->seq[i] != NULL; i++) {
		// creation de pipe dans le pere 
		if(i < nbPipes){
			if (pipe(fd[i]) != 0) {
				perror("pipe");
				exit(EXIT_FAILURE); // Quitte le programme en cas d'erreur de pipe
        	}
			numPipe ++;
		}

		if((childpid = Fork()) == 0){ //fils
			redirectIOPipe(i , nbPipes, fd);
			execCmdWithPipe(l,i);
			// quit le processus fils 
            exit(0);

		}else{ //pere
			// fermer le pipe creer /* chaque fois on cree un pipe pour qu'il soit dupliquer et utiliser dans le fils on le ferme directement dans le pere on ferme le pipe une fois qu'il est deja utiliser par le fils c'est pour ca qu'on fait le decalage de i-1*/
			if (i != 0){
				close(fd[i-1][1]);
			}
  
            Waitpid(childpid,NULL,0);
		}
	}
}




int main()
{
	cmdline *l;
	while(1){
		printf("shell> ");
		l = readcmd();
		affichage(l);
		//exeCmdInterne(l,0);
		pipeCommande(l);
	}
}


/*
bool redirectionEntree(){

}

bool redirectionSortie(){
	
}

void sigInt (int interruption){

}
void sigChildHandler(int interruption){

}*/