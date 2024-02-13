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

void errorFork(pid_t pid){
	if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE); // Quitte le programme en cas d'erreur de fork
    }
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

void execCmdExterne(struct cmdline *l , int i){
    int status;
    pid_t pid = fork();
	errorFork(pid); 
	
    if (pid == 0){ // c'est le fils on va executer la commande
        execvp(l->seq[i][0], l->seq[i]);
        perror("execvp"); // En cas d'erreur d'execution de la commande
        exit(EXIT_FAILURE);
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


void pipeCommande(cmdline *l){
	int n = nbCmd(l);
	int fd[n][2]; // cree n descripteurs de fichiers 

	createPipes(fd , n);
	for (int i=0 ; i<n ; i++){ 
		if(i == 0){ // premier processus
			close(fd[0][0]);
			dup2(fd[0][1] , STDOUT);
			close(fd[0][1]);
			execCmdExterne(l,i);
		}else if(i == n-1){ // dernier processus
			close(fd[i][1]);
			dup2(fd[i][0] , STDIN);
			close(fd[i][0]);
			execCmdExterne(l,i);
		}else{ //processus intermediaires
			//closeUnusedPipes();
			dup2(fd[i-1][0] , STDIN);
			dup2(fd[i][1] , STDOUT);
			// fermer les deux qui reste 
			
			execCmdExterne(l,i);
			
		}
			
		
	}

	//fermer tous les pipes pour le pere 
	closeAllPipe();
	
	while(wait(NULL) != -1){

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



int main()
{
		cmdline *l;

		printf("shell> ");
		l = readcmd();
		//affichage(l);
		//exeCmdInterne(l,0);
		pipeCommande(l);
		

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