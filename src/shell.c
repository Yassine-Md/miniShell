#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <signal.h>
#include "readcmd.h"
#include "utils.h"
#include "commandes_internes.h"
#include "csapp.h"
#include "job.h"
#include "pipe.h"
#include "handler.h"


extern volatile int foregroundProcessCompleted ;

// Fonction pour reprendre un job en background
void bgCommand(int jobId) {
    // Mettre en arriere-plan le processus correspondant au jobNumber
    //system
}


pid_t childpid;

char command[100];
Job jobs[MAX_JOBS]; // tableau des jobs
int numJobs = 0; // la derniere commande mise en arriere-plan

// Fonction pour reprendre un job en foreground
void bringToForeground(int jobId ){
    // Mettre en avant-plan le processus correspondant au jobNumbe
    // la commande fg si une commande en pause ou en backgroun si on fait bg num de son job elle est mis en forground

    foregroundProcessCompleted = 0; // pour blocker le forground et attendre jusqu'a la fin du processus
    if (kill(jobs[jobId].pid, SIGCONT) == -1) { // envoyer un signial SIGCOUNT pour que le processus se remet en mode R+
        perror("kill");
    }
    while (!foregroundProcessCompleted) {
        sleep(1);  // Attendre jusqu'à ce que le gestionnaire de signal marque la fin du processus en avant-plan
    }
    
}

int main() {
    Signal(SIGINT, SIG_IGN); // ignorer le sigint dans le pere
    Signal(SIGCHLD, SigChildHandler); 
    Signal(SIGTSTP, handlerCtrlZ);

    cmdline *l;
    while (1) {
        printf("shell> ");
        l = readcmd();
        affichage(l);

        if (l->seq[0] != NULL) {
            if(strcmp(l->seq[0][0], "quit") == 0){
                exit(0);
            }else if (strcmp(l->seq[0][0], "jobs") == 0) {
                printJobs(l);
            }else if (strcmp(l->seq[0][0], "fg") == 0){
                printf(" Processus %d est mis en forground ...\n" , jobs[atoi(l->seq[0][1])].pid);
                bringToForeground(atoi(l->seq[0][1]));
            }else if (strcmp(l->seq[0][0], "bg") == 0){

            }else {
                pipeCommande(l);
            }
        }
    }

    return 0;
}
