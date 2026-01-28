#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jobs.h"

job* jobs_list[20] = {NULL}; // intilize to null ie no present jobs
job* current_foreground_job = NULL; // track current foreground job

void add_job(int pgid, int status, int fg, char* job_name) { // add job to array of 20 jobs
	for (int i = 0; i < 20; i++) {
		if (jobs_list[i] == NULL) {
			job* j = malloc(sizeof(job));
			j->job_id = i + 1;
			j->pgid = pgid;
			j->status = status;
			
			if (job_name) {
				j->job_name = strdup(job_name);
			} else {
				j->job_name = NULL;
			}

			if (fg == 1) {
				set_job_foreground(pgid);
			} else {
				j->fg = 0;
			}

			jobs_list[i] = j;
			return;
		}
	}
}
// remove job from jobs list based on job id
void remove_job(int job_id) {
	for (int i = 0; i < 20; i++) {
		if (jobs_list[i] && jobs_list[i]->job_id == job_id) {
			free(jobs_list[i]->job_name);
			free(jobs_list[i]);
			if (current_foreground_job == jobs_list[i]) {
				current_foreground_job = NULL;
			}
			jobs_list[i] = NULL;
			return;
		}
	}
}

// output the current jobs in the jobs list
void output_jobs_list() {
	for (int i = 0; i < 20; i++)
	{
		char fg_bg = '-';
		if (jobs_list[i] != NULL) {
			char* status_str;
			if (jobs_list[i]->status == RUNNING) {
				status_str = "Running";
			} else if (jobs_list[i]->status == STOPPED) {
				status_str = "Stopped";
			} else {
				status_str = "";
			}
			if (jobs_list[i] == current_foreground_job && jobs_list[i]->fg == 1) {
				fg_bg = '+';
			} 
			printf("[%d]%c  %s\t%s\n", jobs_list[i]->job_id, fg_bg, status_str, jobs_list[i]->job_name);
		}
	}
}

// set job to foreground or background based on fg value
void set_job_foreground(int pgid) {
	for (int i = 0; i < 20; i++)
	{
		if (jobs_list[i] != NULL && jobs_list[i]->fg == 1) {
			jobs_list[i]->fg = 0; // set other foreground job to background
		}
	}
	for (int i = 0; i < 20; i++) {
		if (jobs_list[i] != NULL && jobs_list[i]->pgid == pgid) {
			jobs_list[i]->fg = 1; // set this job to foreground
			current_foreground_job = jobs_list[i];
			return;
		}
	}
}

void set_job_background(int pgid) {
	for (int i = 0; i < 20; i++) {
		if (jobs_list[i] != NULL && jobs_list[i]->pgid == pgid) {
			jobs_list[i]->fg = 0; // set this job to background
			if (current_foreground_job == jobs_list[i]) {
				current_foreground_job = NULL;
			}
			return;
		}
	}
}