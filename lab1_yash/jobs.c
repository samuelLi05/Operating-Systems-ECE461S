#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jobs.h"

job* jobs_list[20] = {NULL}; // intilize to null ie no present jobs

void add_job(int pgid, int status, char* job_name) { // add job to array of 20 jobs
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
			jobs_list[i] = NULL;
			return;
		}
	}
}