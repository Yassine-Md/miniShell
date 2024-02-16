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
	closeAllPipe(fd , n);
	
	while(wait(NULL) != -1){

	}
}



int main()
{
		cmdline *l;
	while(1){
		printf("shell> ");
		l = readcmd();
		//affichage(l);
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