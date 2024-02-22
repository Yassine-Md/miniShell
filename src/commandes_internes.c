#include "csapp.h"
#include "commandes_internes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tableau des commandes interne
char *cmdInterne[nbCmdInterne] = {"cd" , "export", "pws"};


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
    pid_t pid = fork();
	errorFork(pid); 
	
    if (pid == 0){ // c'est le fils on va executer la commande
        execvp(l->seq[i][0], l->seq[i]);
        perror("execvp"); // En cas d'erreur d'execution de la commande
        exit(EXIT_FAILURE);
    }
}

/*
void SigChildHandler(int sig){
    pid_t pid ;
    while((pid = waitpid(-1,NULL,WNOHANG)) > 0){
        printf("child num : %d\n" , (int)(pid));
    }
    printf("erreur il n'y a plus de fils \n");
    return ;
}*/


void execCmdWithPipe(struct cmdline *l , int i){

    execvp(l->seq[i][0], l->seq[i]);
    perror("execvp"); // En cas d'erreur d'execution de la commande
    printf("ERROR au niveau de la commade %s\n" , l->seq[i][0]);
    exit(EXIT_FAILURE);

}

void redirection_in_out (struct cmdline *l){
     if (l->out != NULL){
            int num = open(l->out,O_WRONLY|O_CREAT,0644);//verifier si le num n'est pas la strdout
            dup2(num, 1);                   //O_CREAT pour verifier si le fichier exeistant ou pas   // definir les premissions pour la creation
        } // c'est le fils on va executer la commande
        if (l->in !=NULL){
            int num = open(l->in, O_RDONLY);
            dup2(num, 0);
        }
}