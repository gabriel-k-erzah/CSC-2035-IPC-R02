/*
 * Replace the following string of 0s with your student number
 * 240242385 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "joblog.h"

/* 
 * DO NOT EDIT the new_log_name function. It is a private helper 
 * function provided for you to create a log name from a process 
 * descriptor for use when reading, writing and deleting a log file.
 * 
 * You must work out what the function does in order to use it properly
 * and to clean up after use.
 */
static char* new_log_name(proc_t* proc) {
    static char* joblog_name_fmt = "%s/%.31s%07d.txt";
                                // string format for the name of a log file
                                // declared static to have only one instance

    if (!proc)
        return NULL;

    char* log_name;
            
    asprintf(&log_name, joblog_name_fmt, JOBLOG_PATH, proc->type_label,
        proc->id);

    return log_name;
}

/* 
 * DO NOT EDIT the joblog_init function that sets up the log directory 
 * if it does not already exist.
 */
int joblog_init(proc_t* proc) {
    int saved_errno = errno;

    if (!proc) {
        errno = EINVAL;
        return -1;
    }
        
    int r = 0;
    if (proc->is_init) {
        struct stat sb;
    
        if (stat(JOBLOG_PATH, &sb) != 0) {
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }  else if (!S_ISDIR(sb.st_mode)) {
            unlink(JOBLOG_PATH);
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }
    }

    joblog_delete(proc);
    
    return r;
}

job_t* joblog_read(proc_t* proc, int entry_num, job_t* job) {
    if (!proc || entry_num < 0) {
        return NULL;
    }

    int saved_errno = errno;

    char* log_name = new_log_name(proc);
    if (!log_name) {
        errno = saved_errno;
        return NULL;
    }

    FILE* fp = fopen(log_name, "r");
    if (!fp) {
        free(log_name);
        errno = saved_errno;
        return NULL;
    }

    long offset = (long)entry_num * JOB_STR_SIZE;
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        free(log_name);
        errno = saved_errno;
        return NULL;
    }

    char buf[JOB_STR_SIZE + 1];
    size_t nread = fread(buf, 1, JOB_STR_SIZE, fp);
    fclose(fp);
    free(log_name);

    if (nread != (size_t)JOB_STR_SIZE) {
        errno = saved_errno;
        return NULL;
    }

    if (buf[JOB_STR_SIZE - 1] != '\n') {
        errno = saved_errno;
        return NULL;
    }
    buf[JOB_STR_SIZE - 1] = '\0';

    job_t* result = str_to_job(buf, job);
    if (!result) {
        errno = saved_errno;
        return NULL;
    }

    errno = saved_errno;
    return result;
}

void joblog_write(proc_t* proc, job_t* job) {

    if (!proc || !job) {
        return;
    }

    int saved_errno = errno;

    char* log_name = new_log_name(proc);
    if (!log_name) {
        errno = saved_errno;
        return;
    }

    FILE* fp = fopen(log_name, "a");
    if (!fp) {
        free(log_name);
        errno = saved_errno;
        return;
    }

    /* Get string representation of job (without newline) */
    char* jstr = job_to_str(job, NULL);
    if (!jstr) {
        fclose(fp);
        free(log_name);
        errno = saved_errno;
        return;
    }

    size_t len = strlen(jstr);

    if (len != (size_t)(JOB_STR_SIZE - 1)) {
        free(jstr);
        fclose(fp);
        free(log_name);
        errno = saved_errno;
        return;
    }


    size_t nw1 = fwrite(jstr, 1, len, fp);
    size_t nw2 = fwrite("\n", 1, 1, fp);

    free(jstr);
    fclose(fp);
    free(log_name);

    if (nw1 != len || nw2 != 1) {
        errno = saved_errno;
        return;
    }

    errno = saved_errno;
}

void joblog_delete(proc_t* proc) {
    if (!proc) {
        return;
    }

    int saved_errno = errno;

    char* log_name = new_log_name(proc);
    if (!log_name) {
        errno = saved_errno;
        return;
    }

    (void)unlink(log_name);

    free(log_name);
    errno = saved_errno;
}