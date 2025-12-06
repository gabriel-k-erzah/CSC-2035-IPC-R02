/*
 * Replace the following string of 0s with your student number
 * 240242385
 */
#include <stdlib.h>
#include <stdbool.h>
#include "pri_jobqueue.h"

/* 
 * Allocate and initialize a job queue on the heap.
 * Similar pattern to job_new in job.c
 */
pri_jobqueue_t* pri_jobqueue_new() {
    pri_jobqueue_t* pjq = (pri_jobqueue_t*) malloc(sizeof(pri_jobqueue_t));
    if (pjq) {
        pri_jobqueue_init(pjq);
    }
    return pjq;
}

/* 
 * Initialize queue: set size to 0, buf_size to capacity, init all jobs.
 */
void pri_jobqueue_init(pri_jobqueue_t* pjq) {
    if (!pjq) {
        return;
    }
    
    pjq->buf_size = JOB_BUFFER_SIZE;
    pjq->size = 0;
    
    /* Initialize all job slots to unused state (priority 0) */
    for (int i = 0; i < JOB_BUFFER_SIZE; i++) {
        job_init(&pjq->jobs[i]);
    }
}

/* 
 * Remove and return highest priority job from queue.
 * Scan buffer for job with lowest priority value (1=highest).
 * If multiple jobs have same priority, return first found (FIFO).
 */
job_t* pri_jobqueue_dequeue(pri_jobqueue_t* pjq, job_t* dst) {
    if (!pjq || pjq->size == 0) {
        return NULL;
    }
    
    /* Find highest priority job - scan buffer for valid jobs (priority >= 1) */
    int best_idx = -1;
    unsigned int best_priority = 0;
    
    for (int i = 0; i < pjq->buf_size; i++) {
        unsigned int pri = pjq->jobs[i].priority;
        if (pri >= 1) {
            /* Lower numeric value = higher priority (1 is highest) */
            if (best_idx == -1 || pri < best_priority) {
                best_idx = i;
                best_priority = pri;
            }
        }
    }
    
    if (best_idx == -1) {
        return NULL;
    }
    
    /* Copy the job to dst or allocate new */
    job_t* result = job_copy(&pjq->jobs[best_idx], dst);

    /* Mark slot as empty (init sets priority to 0) and decrement size */
    job_init(&pjq->jobs[best_idx]);
    pjq->size--;

    return result;
}

/* 
 * Add job to queue. Store in FIFO order (find first empty slot).
 * This makes enqueue simpler; dequeue scans for highest priority.
 * Job is copied to the queue.
 */
void pri_jobqueue_enqueue(pri_jobqueue_t* pjq, job_t* job) {
    if (!pjq || !job || pjq->size >= pjq->buf_size) {
        return;
    }
    
    /* Invalid priority - don't enqueue (as per spec, priority 0 is unused) */
    if (job->priority == 0) {
        return;
    }
    
    /* Find first empty slot (priority == 0 means unused/empty) */
    for (int i = 0; i < pjq->buf_size; i++) {
        if (pjq->jobs[i].priority == 0) {
            job_copy(job, &pjq->jobs[i]);
            pjq->size++;
            return;
        }
    }
}
   
/* 
 * Check if queue is empty. NULL queue is considered empty.
 */
bool pri_jobqueue_is_empty(pri_jobqueue_t* pjq) {
    if (!pjq) {
        return true;
    }
    return pjq->size == 0;
}

/* 
 * Check if queue is full. NULL queue is considered full.
 */
bool pri_jobqueue_is_full(pri_jobqueue_t* pjq) {
    if (!pjq) {
        return true;
    }
    return pjq->size >= pjq->buf_size;
}

/* 
 * Return copy of highest priority job without removing it.
 * Similar logic to dequeue but doesn't remove the job.
 */
job_t* pri_jobqueue_peek(pri_jobqueue_t* pjq, job_t* dst) {
    if (!pjq || pjq->size == 0) {
        return NULL;
    }
    
    /* Find highest priority job - scan buffer for valid jobs (priority >= 1) */
    int best_idx = -1;
    unsigned int best_priority = 0;
    
    for (int i = 0; i < pjq->buf_size; i++) {
        unsigned int pri = pjq->jobs[i].priority;
        if (pri >= 1) {
            /* Lower numeric value = higher priority (1 is highest) */
            if (best_idx == -1 || pri < best_priority) {
                best_idx = i;
                best_priority = pri;
            }
        }
    }
    
    if (best_idx == -1) {
        return NULL;
    }
    
    /* Copy and return the job (don't remove it) */
    return job_copy(&pjq->jobs[best_idx], dst);
}

/* 
 * Return number of jobs in queue. NULL queue has size 0.
 */
int pri_jobqueue_size(pri_jobqueue_t* pjq) {
    if (!pjq) {
        return 0;
    }
    return pjq->size;
}

/* 
 * Return available space in queue. NULL queue has space 0.
 */
int pri_jobqueue_space(pri_jobqueue_t* pjq) {
    if (!pjq) {
        return 0;
    }
    return pjq->buf_size - pjq->size;
}

/* 
 * Free queue allocated by pri_jobqueue_new.
 */
void pri_jobqueue_delete(pri_jobqueue_t* pjq) {
    if (pjq) {
        free(pjq);
    }
}
