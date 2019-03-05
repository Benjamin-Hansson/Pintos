#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "stdbool.h"
#include "threads/thread.h"

#define PTR_ERROR ((tid_t) -1)
typedef int tid_t;
typedef int pid_t;

void validate_char(char *c);
void validate_buffer(void *buffer, unsigned size);
void validate_ptr(void *f, struct thread *t);
void syscall_init (void);
void halt(void);
bool create(const char *file, unsigned initial_size);
int open(const char *file);
int write(int fd, const char *buffer, unsigned size);
void close(int fd);
int read_from_console(void* buffer, unsigned size);
int read (int fd, void *buffer, unsigned size);
void exit(int exit_status);
tid_t exec(const char *file);
int wait(pid_t pid);
#endif /* userprog/syscall.h */
