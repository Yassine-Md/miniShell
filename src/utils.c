
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Fonction pour créer les pipes
void createPipes(int fd[][2], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (pipe(fd[i]) != 0) {
            perror("pipe");
            exit(EXIT_FAILURE); // Quitte le programme en cas d'erreur de pipe
        }
    }
}

void closeUnusedPipes(int fd[][2], int n, int currentPipe) {
    for (int j = 0; j < n; j++) {
		if(j == currentPipe - 1){ // pipe devant la commande 
			// fermer une seul cote cote 
		}else if(j == currentPipe){ // pipe qui precede la commande
			// fermer une seul cote cote 
		}else{ 
            close(fd[j][0]);  // ferme le cote lecture des pipes precedents
            close(fd[j][1]);  // ferme le cote ecriture des pipes precedents
        }
    }
}

// Fonction pour fermer tous les descripteurs de fichiers des pipes
void closeAllPipe(int fd[][2], int n) {
    for (int i = 0; i < n; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }
}

void affichage(cmdline *l){
	int i, j;
/* If input stream closed, normal termination */
	if (!l) {
		printf("exit\n");
		exit(0);
	}

	if (l->err) {
		/* Syntax error, read another command */
		printf("error: %s\n", l->err);
		//continue;
	}

	if (l->in) printf("in: %s\n", l->in);
	if (l->out) printf("out: %s\n", l->out);

	/* Display each command of the pipe */
	for (i=0; l->seq[i]!=0; i++) {
		char **cmd = l->seq[i];
		printf("seq[%d]: ", i);
		for (j=0; cmd[j]!=0; j++) {
			printf("%s ", cmd[j]);
		}
		printf("\n");
	}
}