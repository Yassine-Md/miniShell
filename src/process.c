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


int findProcessIndex(pid_t childpid) {
    for (int i = 0; i < numProcess; i++) {
        if (process[i].pid == childpid) {
            return i;
        }
    }
    return -1;
}

void removeProcess(pid_t childpid) {
    int index = findProcessIndex(childpid);
    if (index >= 0 && index < numProcess) {
        // Marquer le processus comme termine
        process[index].pid = 0;
        for (int i = index; i < numProcess - 1; i++) {
            process[i] = process[i + 1];
        }
        numProcess--;
    }
}


int waitForForegroundProcess(Process *process) { 
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