/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "readcmd.h"
#include "csapp.h"
#include <unistd.h>


#define nbCmdInterne 3
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// tableau des commandes interne
char *cmdInterne[nbCmdInterne] = {"cd" , "export", "pws"};




struct process {
	pid_t pid ;
	pid_t gpid;
	int fg;
};

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

bool isInterne(char *cmd){
	for(int i = 0 ; i < nbCmdInterne ; i++){
		if(strcmp(cmd , cmdInterne[i]) == 0){
			return true;
		}
	}
	return false;
}

void errorFork(){
	fprintf(stderr , "ERROR creation de FILS! \n");
}

int nbCmd(cmdline *l){
	int cpt = 0;
	while(l->seq[cpt] != NULL){
		cpt ++;
	}
	return cpt;
}

void exeCmdInterne(cmdline *l, int i) {
    char *cmd = l->seq[i][0];

    if (strcmp(cmd, "cd") == 0) {
        char *path = l->seq[i][1];
        chdir(path);
    } else if (strcmp(cmd, "pwd") == 0) {
        system("pwd");
    } else if (strcmp(cmd, "export") == 0) {
        // 
    } else {
        printf("ERROR commande introvable!\n");
    }
}

void execCmdExterne(struct cmdline *l){
    int status;
    pid_t pid = fork();
    if (pid == -1){ errorFork(); }
    else if (pid == 0){ // c'est le fils on va executer la commande
        execvp(l->seq[0][0], l->seq[0]);
        exit(0);
    }
    else{ // c'est le pere on va attendre la mort du fils 
        waitpid(-1,&status,0);
    }
}

void pipeCommande(cmdline *l){
	int fd[2];

	pipe(fd);
	pid_t childpid;
	// si on a deux commandes externe 
	//--------------------------------

	//1er cmd
	if ((childpid = Fork()) == 0){ // childs
		close(fd[0]);
		// Redirige la sortie standard vers le cote ecriture du pipe
		dup2(fd[1] , STDOUT);
		close(fd[1]);
		//execution
		execvp(l->seq[0][0] , l->seq[0]);
		
	}
	//2eme cmd
	if ((childpid = Fork()) == 0){ // childs
		close(fd[1]);
		// Redirige la sortie du pipe vers l'entree standard 
		dup2(fd[0] , STDIN);
		close(fd[0]);
		//execution
		execvp(l->seq[1][0] , l->seq[1]);
		
	}
	//pere
	close(fd[0]);
	close(fd[1]);

	// Attend que les deux processus fils se terminent
    wait(NULL);
    wait(NULL);

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



int main()
{
		cmdline *l;

		printf("shell> ");
		l = readcmd();
		//affichage(l);
		//exeCmdInterne(l,0);
		pipeCommande(l);
		

}

