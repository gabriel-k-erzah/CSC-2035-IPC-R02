/*
 * Replace the following string of 0s with your student number
 * 240242385
 */
#include "ipc_jobqueue.h"

/* 
 * DO NOT EDIT the ipc_jobqueue_new function.
 */
ipc_jobqueue_t* ipc_jobqueue_new(proc_t* proc) {
    ipc_jobqueue_t* ijq = ipc_new(proc, "ipc_jobq", sizeof(pri_jobqueue_t));
    
    if (!ijq) 
        return NULL;
    
    if (proc->is_init)
        pri_jobqueue_init((pri_jobqueue_t*) ijq->addr);
    
    return ijq;
}

/* 
 * Wrapper for pri_jobqueue_dequeue with critical work simulation.
 */
job_t* ipc_jobqueue_dequeue(ipc_jobqueue_t* ijq, job_t* dst) {
    if (!ijq) {
        return NULL;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    return pri_jobqueue_dequeue((pri_jobqueue_t*) ijq->addr, dst);
}

/* 
 * Wrapper for pri_jobqueue_enqueue with critical work simulation.
 */
void ipc_jobqueue_enqueue(ipc_jobqueue_t* ijq, job_t* job) {
    if (!ijq) {
        return;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    pri_jobqueue_enqueue((pri_jobqueue_t*) ijq->addr, job);
}
    
/* 
 * Wrapper for pri_jobqueue_is_empty with critical work simulation.
 */
bool ipc_jobqueue_is_empty(ipc_jobqueue_t* ijq) {
    if (!ijq) {
        return true;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    return pri_jobqueue_is_empty((pri_jobqueue_t*) ijq->addr);
}

/* 
 * Wrapper for pri_jobqueue_is_full with critical work simulation.
 */
bool ipc_jobqueue_is_full(ipc_jobqueue_t* ijq) {
    if (!ijq) {
        return true;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    return pri_jobqueue_is_full((pri_jobqueue_t*) ijq->addr);
}

/* 
 * Wrapper for pri_jobqueue_peek with critical work simulation.
 */
job_t* ipc_jobqueue_peek(ipc_jobqueue_t* ijq, job_t* dst) {
    if (!ijq) {
        return NULL;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    return pri_jobqueue_peek((pri_jobqueue_t*) ijq->addr, dst);
}

/* 
 * Wrapper for pri_jobqueue_size with critical work simulation.
 */
int ipc_jobqueue_size(ipc_jobqueue_t* ijq) {
    if (!ijq) {
        return 0;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    return pri_jobqueue_size((pri_jobqueue_t*) ijq->addr);
}

/* 
 * Wrapper for pri_jobqueue_space with critical work simulation.
 */
int ipc_jobqueue_space(ipc_jobqueue_t* ijq) {
    if (!ijq) {
        return 0;
    }
    
    /* Inject critical work delay for simulation */
    do_critical_work(ijq->proc);
    
    /* Call pri_jobqueue function on shared memory queue */
    return pri_jobqueue_space((pri_jobqueue_t*) ijq->addr);
}

/* 
 * Delete ipc_jobqueue. Calls ipc_delete to clean up shared memory.
 */
void ipc_jobqueue_delete(ipc_jobqueue_t* ijq) {
    ipc_delete(ijq);
}
