#include "pipe.h"

void closePipeEndWithErrorCheck(int end, int *pipeEnd) {
    if (close(pipeEnd[end]) == -1) {
        perror("close pipe end");
        exit(EXIT_FAILURE);
    }
}

void dup2WithErrorCheck(int oldfd, int newfd) {
    if (dup2(oldfd, newfd) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
}

void redirectInput(int pipeIndex, int **fd) {
    if (pipeIndex != 0) { // Si ce n'est pas la premire commande
        closePipeEndWithErrorCheck(1, fd[pipeIndex - 1]);
        dup2WithErrorCheck(fd[pipeIndex - 1][0], STDIN_FILENO);
        closePipeEndWithErrorCheck(0, fd[pipeIndex - 1]);
    }
}

void redirectOutput(int pipeIndex, int nbPipes, int **fd) {
    if (pipeIndex != nbPipes) { // Si ce n'est pas la derniere commande
        closePipeEndWithErrorCheck(0, fd[pipeIndex]);
        dup2WithErrorCheck(fd[pipeIndex][1], STDOUT_FILENO);
        closePipeEndWithErrorCheck(1, fd[pipeIndex]);
    }
}

void redirectIOPipe(int cmdCourant, int nbPipes, int **fd) {
    redirectInput(cmdCourant, fd);
    redirectOutput(cmdCourant, nbPipes, fd);
}

// Fonction pour lancer un processus fils
void launchChild(cmdline *l, int i, int numPipe, int nbPipes, int **fd , pid_t *childpid , Job *jobs) {
    *childpid = Fork();
    if (*childpid == 0) { // Fils
        Signal(SIGINT, handlerSigInt);

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

void waitForProcesses(Process *process) {
    while (waitForForegroundProcess(process)) {
        sleep(1);
    }
}


void launchChildProcesses(cmdline *l, int numPipe, int nbPipes, int **fd, pid_t *childpid, Job *jobs) {
    for (int i = 0; l->seq[i] != NULL; i++) {
        launchChild(l, i, numPipe, nbPipes, fd, childpid, jobs);

        if (i != 0) {
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
        }
    }
}

void pipeCommande(cmdline *l, pid_t *childpid, Process *process, Job *jobs) {
    int n = nbCmd(l);
    int nbPipes = n - 1;

    int **fd = allocateDescripteurs(nbPipes);

    for (int i = 0; i < nbPipes; i++) {
        pipe(fd[i]);
    }

    
    launchChildProcesses(l, nbPipes, nbPipes, fd, childpid, jobs);

    waitForProcesses(process);

    freeDescripteurs(fd, nbPipes);
}


void closeAllPipe(int **fd , int nbPipe) {
    for (int j = 0; j < nbPipe; j++) {
        close(fd[j][0]);
        close(fd[j][1]);
    }
}
