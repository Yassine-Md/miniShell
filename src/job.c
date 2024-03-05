#include "job.h"
#include <stdio.h>

extern int numJobs;
extern Job jobs[MAX_JOBS];

void removeJob(int index) {
    if (index >= 0 && index < numJobs) {
        for (int i = index; i < numJobs - 1; i++) {
            jobs[i] = jobs[i + 1];
        }
        numJobs--;
    }
}

void addJob(pid_t pid, char* command) {
    // verifeir qu'il y a pas des jobs plus que c'est permis
    if (numJobs < MAX_JOBS) {
        jobs[numJobs].pid = pid;

        // Utilisez WIFSTOPPED pour verifier si le processus est en pause
        if (WIFSTOPPED(jobs[numJobs].status)) {
            jobs[numJobs].status = JOB_STATUS_STOPPED; 
        }else{ // running
            jobs[numJobs].status = JOB_STATUS_RUNNING;
        }
        // pour des fin de securite on utilise strncpy
        strncpy(jobs[numJobs].command, command, MAX_COMMAND_LENGTH);
        numJobs++;
    } else {
        fprintf(stderr, "Nombre maximal de jobs atteint\n");
    }
}


void printJobs() {
    for (int i = 0; i < numJobs; i++) {
            printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}

