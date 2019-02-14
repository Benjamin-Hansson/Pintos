#ifndef DEVICES_TIMER_H
#define DEVICES_TIMER_H

#include <round.h>
#include <stdint.h>
#include <list.h>
#include "threads/synch.h"

/* Number of timer interrupts per second. */
#define TIMER_FREQ 100


// Queue for sleeping processes
struct sleeping_process {
	struct thread *thread;
	struct list_elem elem;
	struct semaphore *s;
  int64_t target_ticks;
};

void timer_init (void);
void timer_calibrate (void);

int64_t timer_ticks (void);
int64_t timer_elapsed (int64_t);

void timer_sleep (int64_t ticks);
void update_sleeping_processes(void);
bool compare_sleeping_processes(const struct list_elem *a, const struct list_elem *b, void *aux);
void timer_msleep (int64_t milliseconds);
void timer_usleep (int64_t microseconds);
void timer_nsleep (int64_t nanoseconds);

void timer_print_stats (void);

#endif /* devices/timer.h */
