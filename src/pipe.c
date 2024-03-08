#include "pipe.h"

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


void Pipe(int *fd) {
    if (pipe(fd) != 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}


// Fonction pour lancer un processus fils
void launchChild(cmdline *l, int i, int numPipe, int nbPipes, int **fd , pid_t *childpid , Job *jobs) {
    *childpid = Fork();
    if (*childpid == 0) { // Fils
        Signal(SIGINT, handlerSigInt);

        if (l->background) {
            //printf("Processus %d en cours d'exécution en arrière-plan...\n", getpid());
        }

        redirectIOPipe(i, nbPipes, fd);
        closeAllPipe(fd, numPipe);

        execCmdWithPipe(l, i);
        exit(0);
    }else{// pere
        if(!l->background){
            addProcess(*childpid, l->seq[i][0]);
        }else{
            addJob(*childpid , l->seq[i][0],jobs);
        }
    }
}

void pipeCommande(cmdline *l , pid_t *childpid , Process *process , Job *jobs) {
    int n = nbCmd(l);
    int nbPipes = n - 1;
    int numPipe = 0;

    int** fd = allocateDescripteurs(nbPipes);

    for (int i = 0; l->seq[i] != NULL; i++) {
        if (i < nbPipes) {
            pipe(*fd);
            numPipe++;
        }
        launchChild(l, i, numPipe, nbPipes, fd , childpid , jobs);
        if (i != 0) {
            close(fd[i-1][1]);
        }
        
    }
    while(waitForForegroundProcess(process) ){
        sleep(1);
    }

    freeDescripteurs(fd , nbPipes);
}

void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
        close(fd[j][0]);
        close(fd[j][1]);
    }
}

