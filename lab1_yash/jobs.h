/*
Logic for handling jobs in the shell
*/

#ifndef JOBS_H
#define JOBS_H

typedef struct job {
    int job_id;
    int pgid;
    int status;
    char* job_name;
} job;

job* jobs_list[20]; // max number of jobs running at a time is 20

void add_job(int pgid, int status, char* job_name); 

void remove_job(int job_id);

#endif