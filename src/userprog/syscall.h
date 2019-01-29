#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "stdbool.h"

void syscall_init (void);
void halt(void);
bool create(const char *file, unsigned initial_size);
int open(const char *file);
void write(void);

#endif /* userprog/syscall.h */
