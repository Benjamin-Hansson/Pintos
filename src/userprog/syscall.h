#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "stdbool.h"
typedef int tid_t;
void syscall_init (void);
void halt(void);
bool create(const char *file, unsigned initial_size);
int open(const char *file);
int write(int fd, const char *buffer, unsigned size);
void close(int fd);
int read_from_console(void* buffer, unsigned size);
int read (int fd, void *buffer, unsigned size);
int exit(int status);
tid_t exec(const char *file);
int thread_get_exit_status();
#endif /* userprog/syscall.h */
