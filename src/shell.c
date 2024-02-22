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
typedef struct{
    pid_t pid;        // PID du processus
    int status;       // Statut du processus (terminé, en cours, etc.)
    char command[MAX_COMMAND_LENGTH];  // Commande du processus
} Job;

char command[100];
Job jobs[MAX_JOBS]; // tableau des jobs
int numJobs = 0; // la derniere commande mis en arriere plan




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

// Fonction pour afficher la liste des jobs
void printJobs(Job *jobs , int numJobs) {
    printf("Liste des jobs en cours:\n");
    for (int i = 0; i < numJobs; ++i) {
        printf("Job [%d]: %s (PID %d)\n", i + 1, jobs[i].command, jobs[i].pid);
    }
}


// Handler pour le signal SIGTSTP (Ctrl+Z)
void handlerSigTstp(int sig) {
    // Mettre en pause le processus en cours d'exécution
    if (numJobs < MAX_JOBS) {
        int jobIndex = numJobs++;
        // assignier le pid 
        jobs[jobIndex].pid = getpid();
        printf("Job [%d] (%d) stopped\n", jobIndex + 1, jobs[jobIndex].pid);
        strcpy(jobs[jobIndex].command,command);
        raise(SIGSTOP); // Suspendre le processus actuel
        // Afficher la liste des jobs après l'arrêt du processus
        printJobs(jobs , numJobs);
    } else {
        fprintf(stderr, "Trop de jobs actifs, impossible de suspendre\n");
    }
}

/*
void handlerSigInt(int sig) {
    if (!background) { // si on est dans le forground
        Kill(-foregroundGroup, SIGINT); //tuer tous les processus qui appartient au group de forground  } else {
        printf("tous les processus du foreground sont tuer\n");
    }
}*/

void handlerSigInt(int sig) {
    // Affiche un message indiquant que le processus fils a reçu un signal SIGINT
    fprintf(stderr, "Processus fils %d : Signal SIGINT reçu. Terminaison en cours...\n", getpid());
    
    // Tuer le processus fils en envoyant un signal SIGINT à lui-même
    kill(getpid(), SIGINT);
}

void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
    int nbPipes = n-1 ;
    int numPipe = 0;
    //int fd[n-1][2];

    int** fd = allocateDescripteurs(nbPipes);

    pid_t childpid;
    pid_t pgid = 0; // Initialiser pgid à 0

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
            Signal(SIGINT, handlerSigInt);

            // Modifier le GPID des fils
            if (pgid == 0) {
                // Si c'est le premier fils, utiliser son PID comme nouveau GPID
                pgid = getpid();
                printf("pgid a definir %d\n",pgid);
            }

            if (setpgid(0, pgid) == -1) {
                perror("setpgid");
                exit(EXIT_FAILURE);
            }
            printf("Processus num %d et lui attribuer le gpid : %d  et doit etre %d\n", getpid(), getgid() , pgid);
            

            if(l->background){
                printf("Processus %d en cours d'exécution en arrière-plan...\n" , getpid()); // pq getpid renvoie tjrs le gpid 27908 alors qu'il correspond a rien 
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

            Waitpid(childpid,NULL,0);
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
        printf("le pgid est %d----%d\n",getgid(),getpid());
    }
}


/*
bool redirectionEntree(){

}

bool redirectionSortie(){
    
}


*/