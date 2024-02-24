#include "handler.h"

extern pid_t childpid;
extern int numJobs;
extern Job jobs[MAX_JOBS];

void handlerSigInt(int sig) {
    fprintf(stderr, "Processus fils %d : Signal SIGINT reçu. Terminaison en cours...\n", getpid());
    kill(getpid(), SIGINT);
}



void SigChildHandler(int sig) {
    pid_t pid;
    int status;

    // La boucle while sert e traiter tous les processus fils qui ont change d'etat
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        // Recherche du job correspondant au PID dans le tableau des jobs
        for (int i = 0; i < numJobs; i++) {
            if (jobs[i].pid == pid) {
                jobs[i].status = status;

                // Si le processus fils a ete terminer normalement ou a cause d'un signal
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    // Afficher le message indiquant que le job a  ete terminer
                    printf("[%d] Terminé %s\n", i + 1, jobs[i].command);

                    // Supprimer le job du tableau des jobs
                    removeJob(i);
                }
                // Si le processus fils a  ete arreter
                else if (WIFSTOPPED(status)) {
                    // Afficher le message indiquant que le job a ete arreter
                    printf("[%d] Arrêté %s\n", i + 1, jobs[i].command);
                }

                // On a trouve le job correspondant au PID, on peut sortir de la boucle
                break;
            }
        }
    }
}



void handlerCtrlZ(int sig) {
    // Handle Ctrl+Z in the child processes
    printf("Ctrl+Z received in the parent process. Adding child process %d to jobs.\n" , childpid);    
    // Ajouter le processus fils aux jobs
    addJob(childpid, " ");
    // Envoyer le signal SIGSTOP au processus fils apres l'avoir ajoute aux jobs
    kill(childpid, SIGSTOP);
}




