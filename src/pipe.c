#include "pipe.h"

extern pid_t childpid;
int forground = 0;

void redirectIOPipe(int cmdCourant , int nbPipes, int **fd) {
    if (cmdCourant != 0) { // si ce n'est pas la 1ere commande
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

void pipeCommande(cmdline *l) {
    int n = nbCmd(l);
    int nbPipes = n - 1;
    int numPipe = 0;

    int** fd = allocateDescripteurs(nbPipes);

    for (int i = 0; l->seq[i] != NULL; i++) {
        if (i < nbPipes) {
            if (pipe(fd[i]) != 0) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            numPipe++;
        }
        // Installer le gestionnaire pour le signal SIGTSTP dans le processus principal
        Signal(SIGTSTP, handlerCtrlZ);
        childpid = Fork();   // Stocke le PID du processus fils      
        printf("childpid %d\n",childpid);
        forground = 1; 
        if (childpid == 0) { // fils
            Signal(SIGINT, handlerSigInt);
            // Ajouter cette ligne pour retablir le gestionnaire par defaut pour SIGCHLD
            Signal(SIGCHLD, SIG_DFL);

            if (l->background) {
                printf("Processus %d en cours d'exécution en arrière-plan...\n", getpid());
            }

            redirectIOPipe(i, nbPipes, fd);
            closeAllPipe(fd, numPipe);

            execCmdWithPipe(l, i);
            exit(0);
        }else { // 
            if (l->background) {
                // Parent process - add the job without waiting
                addJob(childpid, l->seq[i][0]);
            }else{
                while(forground){
                    sleep(1);
                }
            }
            
            if (i != 0) {
                close(fd[i-1][1]);
            }
        }
    }
    freeDescripteurs(fd , nbPipes);
}

void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
        close(fd[j][0]);
        close(fd[j][1]);
    }
}