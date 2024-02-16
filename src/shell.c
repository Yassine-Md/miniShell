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


void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
    int fd[n-1][2];

    createPipes(fd, n);
	pid_t childpid;

    for (int i = 0; i < n; i++) {
		if((childpid = Fork()) == 0){ 

			if (i == 0) {
				printf("aa\n");
				closeUnusedPipes(fd , n , i);
				dup2(fd[0][1], STDOUT);
				// execution de la commande 
				// Exécution de la commande
        	} else if (i == n - 1) {
				printf("aaa\n");
				closeUnusedPipes(fd , n , i);
				dup2(fd[i-1][0], STDIN);
				// execution de la commande
				// Exécution de la commande
        	} else {
				printf("aaaa\n");
				closeUnusedPipes(fd , n , i);
				dup2(fd[i-1][1], STDIN);
				dup2(fd[i][0], STDOUT);	
				// execution de la commande	
				// Exécution de la commande
        	}
			execCmdWithPipe(l,i);

		}

    }

    closeAllPipe(fd, n);

    for (int i = 0; i < n; i++) {
        wait(NULL);
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