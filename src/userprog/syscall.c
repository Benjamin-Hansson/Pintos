#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "stdbool.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}






static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");

  // Set variables used
  char p[5] = "file";

  // Save relevant info from f as variables
  int *pointer = f->esp;
  int sys_call_number = *pointer;

  // pointer+2 pointer to third argument etc..

  char *file;
  unsigned initial_size;
  int file_descriptor;


  switch(sys_call_number){
    case(SYS_HALT):
      halt();
      break;
    case(SYS_EXIT):
      break;
    case(SYS_EXEC):
    break;
    case(SYS_WAIT):
    break;
    case(SYS_CREATE):
      file = (char*)(pointer+1);
      initial_size = (unsigned)(pointer+2);
      create(file, initial_size);
      break;
    case(SYS_REMOVE):
    break;
    case(SYS_OPEN):
      file = (char*)(pointer+1);
      open(p);
      break;
    case(SYS_FILESIZE):
    break;
    case(SYS_READ):
    break;
    case(SYS_WRITE):
      file_descriptor = *(pointer+1);
      write();
      break;
    case(SYS_SEEK):
    break;
    case(SYS_TELL):
    break;
    case(SYS_CLOSE):
    break;
    case(SYS_MMAP):
    break;
    case(SYS_MUNMAP):
    break;
    case(SYS_CHDIR):
    break;
    case(SYS_MKDIR):
    break;
    case(SYS_READDIR):
    break;
    case(SYS_ISDIR):
    break;
    case(SYS_INUMBER):
      break;
    default:
      break;
  }
  thread_exit ();
}





void halt(void){
  printf("powering off\n");
  power_off();
}

bool create(const char *file, unsigned initial_size){
  return filesys_create(file, initial_size);
}

int open(const char *file){
  struct thread *thread = thread_current();
  // Find first empty i
  for (int i = 0; i < 128; i++) {
      if (thread->open_files[i] == NULL) {
        // filesys_open returns the new file if successful or a null pointer
        thread->open_files[i] = filesys_open(file);
        if(thread->open_files[i] != NULL){
          return i;
        }
        break;
      }
  }
  // if array full or file can not be opened
  return -1;
}


void write(void){
  printf("komigen nu britt-marie WRITE för F-N\n");
}
