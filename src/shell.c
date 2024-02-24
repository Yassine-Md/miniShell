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





pid_t childpid;

char command[100];
Job jobs[MAX_JOBS]; // tableau des jobs
int numJobs = 0; // la derniere commande mise en arriere-plan



int main() {
    Signal(SIGINT, SIG_IGN); // ignorer le sigint dans le pere
    Signal(SIGCHLD, SigChildHandler); 

    cmdline *l;
    while (1) {
        printf("shell> ");
        l = readcmd();
        affichage(l);

        if (l->seq[0] != NULL) {
            if (strcmp(l->seq[0][0], "jobs") == 0) {
                printJobs();
            } else {
                pipeCommande(l);
            }
        }
    }

    return 0;
}
