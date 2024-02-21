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

#define MAX_JOBS 10
#define MAX_COMMAND_LENGTH 100

// Structure pour stocker les informations sur un job
struct Job {
    pid_t pid;        // PID du processus
    int status;       // Statut du processus (terminé, en cours, etc.)
    char command[MAX_COMMAND_LENGTH];  // Commande du processus
};

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


void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
    	close(fd[j][0]);
        close(fd[j][1]);
    }
}

//cmdCourant c'est le ieme commande en cours
void redirectIOPipe(int cmdCourant , int nbPipes, int **fd) {
    if (cmdCourant != 0) { // si ce n'est pas la 1er commande
        if(dup2(fd[cmdCourant-1][0], STDIN_FILENO) == -1){
			perror("dup2 stdin");
            exit(EXIT_FAILURE);
		}
    }

    if (cmdCourant != nbPipes) { // si ce n'est pas la derniere commande
        if(dup2(fd[cmdCourant][1], STDOUT_FILENO) == -1){
			perror("dup2 stdout");
            exit(EXIT_FAILURE);
		}
    }
}

/*
void handlerSigInt(int sig) {
	if (!background) { // si on est dans le forground
		Kill(-foregroundGroup, SIGINT); //tuer tous les processus qui appartient au group de forground	} else {
		printf("tous les processus du foreground sont tuer\n");
	}
}*/


void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
	int nbPipes = n-1 ;
	int numPipe = 0;
    //int fd[n-1][2];

	int** fd = allocateDescripteurs(nbPipes);

	pid_t childpid;
	pid_t pgid; // Pour stocker le GPID commun

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
			// Rétablir le comportement par défaut de SIGINT
			Signal(SIGINT, SIG_DFL);

			//modifier le gpid des fils (creer un autre groupe qui contient tous les fils creer par ce que setgpid renvoie un erreur de privileges)
			if (setpgid(childpid,11000) == -1) {
				perror("setpgid");
				exit(EXIT_FAILURE);
			}
			printf("Processus %d et lui attribuer le gpid : %d \n", getpid(),getgid());

			if(l->background){
				printf("Processus %d en cours d'exécution en arrière-plan...\n" , getpid());
				Signal(SIGCHLD , SigChildHandler);
			}
			// dans 1er commande uniquement qu'on va rederiger la sortie vers l'entrer du pipe otherwise on deriger la sortie du commande vers la sortie standard
			/*if (i != 0) {
                dup2(fd[i - 1][0], STDIN_FILENO);
            }
			//lorsque ce n'est pas la dernier commande on redirige la sortie standard vers le pipe qui suit 
            if (i != nbPipes) {
               dup2(fd[i][1], STDOUT_FILENO);
            }*/
			redirectIOPipe(i , nbPipes, fd);

            /*for (int j = 0; j < numPipe; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }*/
			closeAllPipe(fd , numPipe);

			/*if (execvp(l->seq[i][0], l->seq[i]) < 0) {
                fprintf(stderr, "%s: command not found\n", l->seq[i][0]);
                exit(EXIT_FAILURE);
            }*/
			execCmdWithPipe(l,i);
            
			// quit le processus fils 
            exit(0);

		}else{ //pere
			// fermer le pipe creer /* chaque fois on cree un pipe pour qu'il soit dupliquer et utiliser dans le fils on le ferme directement dans le pere on ferme le pipe une fois qu'il est deja utiliser par le fils c'est pour ca qu'on fait le decalage de i-1*/
			if (i != 0){
				close(fd[i-1][1]);
			}
			printf("Processus pere %d et lui attribuer le gpid : %d \n", getpid(),getgid());

			// Enregistrez le GPID dans le premier tour de la boucle
            if (i == 0) {
                pgid = childpid;
            }
  
            //Waitpid(childpid,NULL,0);
		}
	}
}




int main()
{
	// le pere ignore le sigInt
	Signal(SIGINT, SIG_IGN);
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


*/