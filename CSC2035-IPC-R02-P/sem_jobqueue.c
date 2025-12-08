/*
 * Replace the following string of 0s with your student number
 * 240242385
 */
#include <fcntl.h>          /* For O_* constants */
#include <sys/stat.h>       /* For mode constants */
#include <semaphore.h>
#include "shobject_name.h"
#include "sem_jobqueue.h"

/* 
 * DO NOT EDIT the following declarations that are used to detect
 * failures in semaphore setup
 */
#define SEM_NEW_FAIL        000
#define MUTEX_SEM_SUCCESS   001
#define FULL_SEM_SUCCESS    002
#define EMPTY_SEM_SUCCESS   004
#define ALL_SEM_SUCCESS     007

/* 
 * DO NOT EDIT the following semaphore names.
 */
static const char* sem_mutex_label = "sjq.mutex";
static const char* sem_full_label = "sjq.full";
static const char* sem_empty_label = "sjq.empty";

/* 
 * DO NOT EDIT the private helper function sem_new for creating new
 * semaphores at initialisation
 */
static int sem_new(sem_t** sem, const char* sem_label, int init_value, 
    int success) {
    char sem_name[MAX_NAME_SIZE];
    
    shobject_name(sem_label, sem_name);
    
    sem_t* new_sem = sem_open(sem_name, O_CREAT, S_IRWXU, init_value);
    
    if (new_sem == SEM_FAILED)
        return SEM_NEW_FAIL;
    
    *sem = new_sem;
        
    return success;
}

/* 
 * DO NOT EDIT the private helper function sem_delete for closing semaphores
 * when a sem_jobqueue is deleted.
 */
static void sem_delete(sem_t* sem, const char* sem_label) {
    char sem_name[MAX_NAME_SIZE];
    sem_close(sem);
    shobject_name(sem_label, sem_name);
    sem_unlink(sem_name);
}

/* 
 * DO NOT EDIT sem_jobqueue_new that creates a new sem_jobqueue_t
 * and associated semaphores.
 * You will need to look at this function to see what needs to be deleted, 
 * freed or closed by sem_jobqueue_delete
 */
sem_jobqueue_t* sem_jobqueue_new(proc_t* proc) {
    sem_jobqueue_t* sjq = (sem_jobqueue_t*) malloc(sizeof(sem_jobqueue_t));

    if (!sjq)
        return NULL;
        
    sjq->ijq = ipc_jobqueue_new(proc);   // delays all but init process
    
    if (!sjq->ijq) {
        free(sjq);
        return NULL;
    }
    
    int r = sem_new(&sjq->mutex, sem_mutex_label, 1, MUTEX_SEM_SUCCESS);
    
    if (r != MUTEX_SEM_SUCCESS) {
        ipc_jobqueue_delete(sjq->ijq);
        free(sjq);
        return NULL;
    }
    
    sem_wait(sjq->mutex);
    
    r |= sem_new(&sjq->full, sem_full_label, 0, FULL_SEM_SUCCESS)
            | sem_new(&sjq->empty, sem_empty_label,
                ipc_jobqueue_space(sjq->ijq), EMPTY_SEM_SUCCESS);
    
    if (r & ALL_SEM_SUCCESS) {
        sem_post(sjq->mutex);
        return sjq;    // all succeeded
    }
    
    // mutex failures    
    if (r & FULL_SEM_SUCCESS)
        sem_delete(sjq->full, sem_full_label);

    if (r & EMPTY_SEM_SUCCESS)
        sem_delete(sjq->empty, sem_empty_label);

    sem_post(sjq->mutex);
    sem_delete(sjq->mutex, sem_mutex_label);
    ipc_jobqueue_delete(sjq->ijq);
    free(sjq);
                
    return NULL;
}

/* 
 * Dequeue with semaphore protection (Monitor pattern).
 * Wait for full (item available), acquire mutex, dequeue, release mutex, signal empty.
 */
job_t* sem_jobqueue_dequeue(sem_jobqueue_t* sjq, job_t* dst) {
    if (!sjq) {
        return NULL;
    }
    
    /* Wait for queue to have at least one item */
    if (sem_wait(sjq->full) != 0) {
        return NULL;
    }
    
    /* Acquire mutex for exclusive access */
    if (sem_wait(sjq->mutex) != 0) {
        return NULL;
    }
    
    /* Perform dequeue operation */
    job_t* result = ipc_jobqueue_dequeue(sjq->ijq, dst);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    /* Signal that queue has one more empty slot */
    sem_post(sjq->empty);
    
    return result;
}

/* 
 * Enqueue with semaphore protection (Monitor pattern).
 * Wait for empty (space available), acquire mutex, enqueue, release mutex, signal full.
 */
void sem_jobqueue_enqueue(sem_jobqueue_t* sjq, job_t* job) {
    if (!sjq) {
        return;
    }
    
    /* Wait for queue to have at least one empty slot */
    if (sem_wait(sjq->empty) != 0) {
        return;
    }
    
    /* Acquire mutex for exclusive access */
    if (sem_wait(sjq->mutex) != 0) {
        return;
    }
    
    /* Perform enqueue operation */
    ipc_jobqueue_enqueue(sjq->ijq, job);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    /* Signal that queue has one more item */
    sem_post(sjq->full);
}

/* 
 * Check if queue is empty with mutex protection.
 */
bool sem_jobqueue_is_empty(sem_jobqueue_t* sjq) {
    if (!sjq) {
        return true;
    }
    
    /* Acquire mutex for reading queue state */
    if (sem_wait(sjq->mutex) != 0) {
        return true;
    }
    
    bool result = ipc_jobqueue_is_empty(sjq->ijq);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    return result;
}

/* 
 * Check if queue is full with mutex protection.
 */
bool sem_jobqueue_is_full(sem_jobqueue_t* sjq) {
    if (!sjq) {
        return true;
    }
    
    /* Acquire mutex for reading queue state */
    if (sem_wait(sjq->mutex) != 0) {
        return true;
    }
    
    bool result = ipc_jobqueue_is_full(sjq->ijq);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    return result;
}

/* 
 * Peek at highest priority job with mutex protection.
 */
job_t* sem_jobqueue_peek(sem_jobqueue_t* sjq, job_t* dst) {
    if (!sjq) {
        return NULL;
    }
    
    /* Acquire mutex for reading queue */
    if (sem_wait(sjq->mutex) != 0) {
        return NULL;
    }
    
    job_t* result = ipc_jobqueue_peek(sjq->ijq, dst);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    return result;
}

/* 
 * Get queue size with mutex protection.
 */
int sem_jobqueue_size(sem_jobqueue_t* sjq) {
    if (!sjq) {
        return 0;
    }
    
    /* Acquire mutex for reading queue state */
    if (sem_wait(sjq->mutex) != 0) {
        return 0;
    }
    
    int result = ipc_jobqueue_size(sjq->ijq);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    return result;
}

/* 
 * Get queue available space with mutex protection.
 */
int sem_jobqueue_space(sem_jobqueue_t* sjq) {
    if (!sjq) {
        return 0;
    }
    
    /* Acquire mutex for reading queue state */
    if (sem_wait(sjq->mutex) != 0) {
        return 0;
    }
    
    int result = ipc_jobqueue_space(sjq->ijq);
    
    /* Release mutex */
    sem_post(sjq->mutex);
    
    return result;
}

/* 
 * Delete sem_jobqueue. Close semaphores, delete ipc_jobqueue, free sjq.
 * Reverses order of allocation in sem_jobqueue_new.
 */
void sem_jobqueue_delete(sem_jobqueue_t* sjq) {
    if (!sjq) {
        return;
    }
    
    /* Close and unlink semaphores in reverse order of creation */
    if (sjq->empty) {
        sem_delete(sjq->empty, sem_empty_label);
    }
    
    if (sjq->full) {
        sem_delete(sjq->full, sem_full_label);
    }
    
    if (sjq->mutex) {
        sem_delete(sjq->mutex, sem_mutex_label);
    }
    
    /* Delete underlying ipc_jobqueue */
    if (sjq->ijq) {
        ipc_jobqueue_delete(sjq->ijq);
    }
    
    /* Free the sem_jobqueue struct itself */
    free(sjq);
}
