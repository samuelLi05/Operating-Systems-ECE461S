/*
Logic for handling jobs in the shell
*/

#ifndef JOBS_H
#define JOBS_H

typedef struct job {
    int job_id;
    int pgid;
    int status;
    int fg; // 1 for foreground, 0 for background
    char* job_name;
} job;

enum job_status {
    RUNNING = 1, 
    STOPPED = 2, 
};

extern job* jobs_list[20]; // max number of jobs running at a time is 20
extern job* current_foreground_job; // track current foreground job
extern job* current_job; // track the most recent job

void add_job(int pgid, int status, int fg, char* job_name); 

void remove_job(int job_id);

void output_jobs_list(); // for jobs command to output current jobs

void set_job_foreground(int pgid); // set job to foreground or background based on fg value

void set_job_background(int pgid); // set job to background

int get_largest_job_id(); // get the largest job id currently in jobs list

void get_current_job();

#endif