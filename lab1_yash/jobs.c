#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jobs.h"

job* jobs_list[20] = {NULL}; // intilize to null ie no present jobs
job* current_foreground_job = NULL; // track current foreground job
job* current_job = NULL; // track the most recent job
job* current_stopped_job = NULL; // track the most recent stopped job

void add_job(int pgid, int status, int fg, char* job_name) { // add job to array of 20 jobs
	int largest_id = get_largest_job_id();
	int new_job_id = largest_id + 1;
	int new_job_index = largest_id; // find first empty slot
	if (new_job_index >= 20) {
		printf("Error: Maximum job limit reached.\n");
		return;
	}
	job* new_job = (job*)malloc(sizeof(job));
	new_job->job_id = new_job_id;
	new_job->pgid = pgid;
	new_job->status = status;
	new_job->fg = fg;
	if (fg == 1) {
		current_foreground_job = new_job;
		set_job_foreground(pgid);
	}
	new_job->job_name = strdup(job_name); // duplicate the job name string

	jobs_list[new_job_index] = new_job;
	current_job = new_job;
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
			if (current_job == jobs_list[i])
			{
				current_job = NULL;
			}
			if (current_stopped_job == jobs_list[i]) {
				current_stopped_job = NULL;
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
		char curr = '-';
		if (jobs_list[i] != NULL) {
			char* status_str;
			if (jobs_list[i]->status == RUNNING) {
				status_str = "Running";
			} else if (jobs_list[i]->status == STOPPED) {
				status_str = "Stopped";
			} else {
				status_str = "";
			}
			if (jobs_list[i] == current_job) {
				curr = '+';
			} 
			printf("[%d]%c  %s\t%s\n", jobs_list[i]->job_id, curr, status_str, jobs_list[i]->job_name);
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

// get the largest job id currently in jobs list
int get_largest_job_id() {
	int largest_id = 0;
	for (int i = 0; i < 20; i++) {
		if (jobs_list[i] != NULL && jobs_list[i]->job_id > largest_id) {
			largest_id = jobs_list[i]->job_id;
		}
	}
	return largest_id;
}

// get the most recent job added to jobs list
void get_current_job() {
	for (int i = 19; i >= 0; i--) {
		if (jobs_list[i] != NULL) {
			current_job = jobs_list[i];
			return;
		}
	}
	current_job = NULL;
}

// get the most recent stopped job added to jobs list
void get_recent_stopped_job() {
	for (int i = 19; i >= 0; i--) {
		if (jobs_list[i] != NULL && jobs_list[i]->status == STOPPED) {
			current_stopped_job = jobs_list[i];
			return;
		}
	}
	current_stopped_job = NULL;
}