/*
 * Replace the following string of 0s with your student number
 * 240242385
 */
#include <stdlib.h>
#include <stdbool.h>
#include "pri_jobqueue.h"
pri_jobqueue_t* pri_jobqueue_new() {
    pri_jobqueue_t* pjq = (pri_jobqueue_t*) malloc(sizeof(pri_jobqueue_t));
    if (pjq) {
        pri_jobqueue_init(pjq);
    }
    return pjq;
}
void pri_jobqueue_init(pri_jobqueue_t* pjq) {
    if (!pjq) {
        return;
    }
    pjq->buf_size = JOB_BUFFER_SIZE;
    pjq->size = 0;
    for (int i = 0; i < JOB_BUFFER_SIZE; i++) {
        job_init(&pjq->jobs[i]);
    }
}
job_t* pri_jobqueue_dequeue(pri_jobqueue_t* pjq, job_t* dst) {
    if (!pjq || pjq->size == 0) {return NULL;}
    int best_idx = -1;
    unsigned int best_priority = 0;
    for (int i = 0; i < pjq->size; i++) {
        unsigned int pri = pjq->jobs[i].priority;
        if (pri == 0) {continue;}              /* unused / invalid */
        if (best_idx == -1 || pri < best_priority) {
            best_idx = i;
            best_priority = pri;
        }
    }

    if (best_idx == -1) {return NULL;}
    job_t* result = job_copy(&pjq->jobs[best_idx], dst);
    if (result == NULL) {return NULL;}
    for (int i = best_idx; i < pjq->size - 1; i++) {pjq->jobs[i] = pjq->jobs[i + 1];}
    pjq->size--;
    job_init(&pjq->jobs[pjq->size]);
    return result;
}
void pri_jobqueue_enqueue(pri_jobqueue_t* pjq, job_t* job) {
    if (!pjq || !job || pjq->size >= pjq->buf_size) {
        return;
    }

    if (job->priority == 0) {return;}
    for (int i = 0; i < pjq->buf_size; i++) {
        if (pjq->jobs[i].priority == 0) {
            job_copy(job, &pjq->jobs[i]);
            pjq->size++;
            return;
        }
    }
}
bool pri_jobqueue_is_empty(pri_jobqueue_t* pjq) {
    if (!pjq) {return true;}
    return pjq->size == 0;
}
bool pri_jobqueue_is_full(pri_jobqueue_t* pjq) {
    if (!pjq) {return true;}
    return pjq->size >= pjq->buf_size;
}
job_t* pri_jobqueue_peek(pri_jobqueue_t* pjq, job_t* dst) {
    if (!pjq || pjq->size == 0) {return NULL;}
    int best_idx = -1;
    unsigned int best_priority = 0;
    for (int i = 0; i < pjq->buf_size; i++) {
        unsigned int pri = pjq->jobs[i].priority;
        if (pri >= 1) {
            if (best_idx == -1 || pri < best_priority) {
                best_idx = i;
                best_priority = pri;
            }
        }
    }
    
    if (best_idx == -1) {return NULL;}
    return job_copy(&pjq->jobs[best_idx], dst);
}
int pri_jobqueue_size(pri_jobqueue_t* pjq) {
    if (!pjq) {return 0;}
    return pjq->size;
}

int pri_jobqueue_space(pri_jobqueue_t* pjq) {
    if (!pjq) {return 0;}
    return pjq->buf_size - pjq->size;
}
void pri_jobqueue_delete(pri_jobqueue_t* pjq) {
    if (pjq) {free(pjq);
    }
}
