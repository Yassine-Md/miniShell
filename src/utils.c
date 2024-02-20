
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "csapp.h"

// Fonction pour créer les pipes
/*void createPipes(int fd[][2], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (pipe(fd[i]) != 0) {
            perror("pipe");
            exit(EXIT_FAILURE); // Quitte le programme en cas d'erreur de pipe
        }
    }
}*/




/*erreur lorsqu'on est dans la 1er commande */
// Fonction pour fermer tous les descripteurs de fichiers des pipes
/*void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
    	close(fd[j][0]);
        close(fd[j][1]);
    }
}*/

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









//structure pour stocker tous les processus qui sont en cours : permet de determiner aussi celle qui sont fg et celle qui sont bg pour gerer la boucle while pour attendre l'execution des processus qui sont en fg


// analyse de la lig de cmd func readcmd modifier pr gerer la presence de & et le traiter a part 
// ainsi que d'ajouter un champs dans la structure pour preciser si le process et fg ou bg 



//afficher la resultat a la fin de tous les commandes donc des wait avant que l'utilisateurs peut retaper des nv commandes



//arrierer plan retounrer la main tt de suite 


// permer de stocker les processus fils et les tracker 
/*struct process {
	// pid 
	// gpid
}*/

/*

struct process {
	pid_t pid ;
	pid_t gpid;
	int fg;
};

*/