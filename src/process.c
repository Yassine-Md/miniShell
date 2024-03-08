#include "process.h"
extern Process process[MAX_PROCESS];
extern int numProcess;

void addProcess(pid_t pid, char* command){
    if (numProcess < MAX_PROCESS) {
        process[numProcess].pid = pid;

            // Utilisez WIFSTOPPED pour verifier si le processus est en pause
        if (WIFSTOPPED(process[numProcess].status)) {
            process[numProcess].status = PROCESS_STATUS_STOPPED; 
        }else{ // running
            process[numProcess].status = PROCESS_STATUS_RUNNING;
        }
        // pour des fin de securite on utilise strncpy
        strncpy(process[numProcess].command, command, MAX_COMMAND_LENGTH);
        numProcess++;
    } else {
        fprintf(stderr, "Nombre maximal de jobs atteint\n");
    }
}

void removeProcess(int index) { 
    if (index >= 0 && index < numProcess) {
        // Marquer le processus comme terminé
        process[index].pid = 0;
        for (int i = index; i < numProcess - 1; i++) {
            process[i] = process[i + 1];
        }
        numProcess--;
    }
}


int waitForForegroundProcess(Process *process) { // garbage value lors de l'allocation du tableau processus 
    int count = 0;
    while (process[count].pid != 0) {
        count++;
    }

    return count;
}

void printProcess() {
    for (int i = 0; i < numProcess; i++) {
            printf("Process : [%d] %d %s\n", i + 1, process[i].pid, process[i].command);
    }
}