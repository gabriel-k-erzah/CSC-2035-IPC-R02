/*
 * Replace the following string of 0s with your student number
 * 240242385
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "job.h"

/* 
 * DO NOT EDIT the job_new function.
 */
job_t* job_new(pid_t pid, unsigned int id, unsigned int priority, 
    const char* label) {
    return job_set((job_t*) malloc(sizeof(job_t)), pid, id, priority, label);
}

job_t* job_copy(job_t* src, job_t* dst) {
    if (src == NULL) {return NULL;}
    if(strlen(src->label) != MAX_NAME_SIZE - 1){return NULL;}
    if (dst == src) {return dst;}
    if (dst == NULL) {job_t* newjob = job_new(src->pid, src->id, src->priority, src->label);return newjob;}
    job_set(dst, src->pid, src->id, src->priority, src->label);
    return dst;
}

void job_init(job_t* job) {
    if (job == NULL) {
        return;
    }

    job->pid = 0;
    job->id = 0;
    job->priority = 0;

    for (size_t i = 0; i < MAX_NAME_SIZE - 1; i++) {
        job->label[i] = '*';
    }
    job->label[MAX_NAME_SIZE - 1] = '\0';
}

bool job_is_equal(job_t* j1, job_t* j2) {
    if (j1 == NULL && j2 == NULL) {
        return true;
    }
    if (j1 == NULL || j2 == NULL) {
        return false;
    }

    if (j1->pid != j2->pid) return false;
    if (j1->id != j2->id) return false;
    if (j1->priority != j2->priority) return false;

    return strcmp(j1->label, j2->label) == 0;
}

job_t* job_set(job_t* job, pid_t pid, unsigned int id, unsigned int priority,
    const char* label) {

    if (job == NULL) {return NULL;}

    job->pid = pid;
    job->id = id;
    job->priority = priority;

    size_t i = 0;
    if (label != NULL && label[0] != '\0') {
        for (i = 0; i < MAX_NAME_SIZE - 1 && label[i] != '\0'; i++) {
            job->label[i] = label[i];
        }
    }

    for (; i < MAX_NAME_SIZE - 1; i++) {
        job->label[i] = '*';
    }
    job->label[MAX_NAME_SIZE - 1] = '\0';
    return job;
}

char* job_to_str(job_t* job, char* str) {
    char* out;
    if(job == NULL){
        return NULL;
    }

    if (strlen(job->label) != MAX_NAME_SIZE - 1) {
        return NULL;
    }

    if (str == NULL) {
        out = malloc(JOB_STR_SIZE);
        if (out == NULL) {
            return NULL;
        }
    } else {
        out = str;
    }

    int written = snprintf(out, JOB_STR_SIZE, JOB_STR_FMT,job->pid, job->id, job->priority, job->label);

    if (written < 0 || written >= JOB_STR_SIZE) {
        if (str == NULL) {
            free(out);
        }
        return NULL;
    }
    return out;
}

job_t* str_to_job(char* str, job_t* job) {
    if (str == NULL) {
        return NULL;
    }

    int pid_tmp;
    unsigned int id_tmp;
    unsigned int priority_tmp;
    char label_buf[MAX_NAME_SIZE];

    int scanned = sscanf(str, JOB_STR_FMT, &pid_tmp, &id_tmp, &priority_tmp, label_buf);

    if (scanned != 4) {
        return NULL;
    }
    if (strlen(label_buf) != MAX_NAME_SIZE - 1) {
        return NULL;
    }
    if (job == NULL) {
        return job_new((pid_t)pid_tmp, id_tmp, priority_tmp, label_buf);
    }
    return job_set(job, (pid_t)pid_tmp, id_tmp, priority_tmp, label_buf);
}

void job_delete(job_t* job) {
    if (job != NULL) {
        free(job);
    }
}