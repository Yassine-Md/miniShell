
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "csapp.h"








void affichage(cmdline *l){
	int i, j;
/* If input stream closed, normal termination */
	if (!l) {
		printf("exit\n");
		exit(0);
	}

	if (l->err) {
		/* Syntax error, read another command */
		printf("error: %s\n", l->err);/*void closeAllPipe(int **fd, int nbPipe);*/

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

void freeDescripteurs(int** fd, int nbPipes) {
    for (int i = 0; i < nbPipes; i++) {
        free(fd[i]);
    }
    free(fd);
}


