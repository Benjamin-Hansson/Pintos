#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "stdbool.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  // printf ("system call!\n");

  // Save relevant info from f as variables
  int *pointer = f->esp;
  int sys_call_number = *pointer;

  // pointer+2 pointer to third argument etc..
  void *buffer;
  unsigned size;
  char *file;
  unsigned initial_size;
  int fd;

  switch(sys_call_number){
    case(SYS_HALT):
      halt();
      break;

    case(SYS_EXIT):
      exit(1);
      break;
    case(SYS_EXEC):
      break;
    case(SYS_WAIT):
      break;

    case(SYS_CREATE):
      file = (char*) *(pointer+1);
      initial_size = (unsigned) *(pointer+2);
      f->eax = create(file, initial_size);
      break;

    case(SYS_REMOVE):
      break;

    case(SYS_OPEN):
      file = (char*) *(pointer+1);
      f->eax = open(file);
      break;

    case(SYS_FILESIZE):
      break;

    case(SYS_READ):
      fd = *(pointer+1);
      buffer = (void*)(pointer+2);
      size = (unsigned) *(pointer +3);
      f->eax = read(fd, buffer, size);
      break;

    case(SYS_WRITE):
      fd = *(pointer+1);
      buffer = (char*)(pointer+2);
      size = (unsigned) *(pointer +3);
      f->eax = write(fd, buffer, size);
      break;

    case(SYS_SEEK):
      break;

    case(SYS_TELL):
      break;

    case(SYS_CLOSE):
      fd = *(pointer+1);
      close(fd);
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

}

void halt(void){
  printf("powering off\n");
  power_off();
}

bool create(const char *file, unsigned initial_size){
  return filesys_create(file, initial_size);
}

int open(const char *file){

  return thread_open_file(file);
}

void close(int fd){
  if(2 <= fd && fd < 130){
    thread_close_file(fd, thread_current());
    }
}


int read (int fd, void *buffer, unsigned size){
  void *buffer2 =  *((void**)buffer); //LIKE A BOSS
  // If 0, then read input from keyboard
  if(fd == 0){
    return read_from_console(buffer2, size);
  }
  // if fd is outside it's range
  if(fd > 129 || fd < 2){
    return -1;
  }
  else{
    struct file *file = thread_get_file(fd);
    if(file == NULL) return -1;
    // file_read reads SIZE bytes from FILE into BUFFER


    int read_bytes = file_read(file, buffer2, size);
    if(read_bytes == 0) return -1;
    return read_bytes;
  }
}

// Reads (size) bytes from keyboard
int read_from_console(void* buffer, unsigned size){

  for(unsigned i = 0; i < size; i++){
      *((uint8_t *)buffer) = input_getc();
      buffer++;
    }
    return size;

}

int write(int fd, const char *buffer, unsigned size){
  // If 1, then write to console
  // printf("%d, fd\n", fd);
  // printf("%u, size med d\n", size);

 //casting and dereferencing like a BOSS
  const char *buffer2 =  *((const char**)buffer);
  if(fd == 1){
    putbuf(buffer2, (size_t)size);
    return size;
  }

  // if fd is outside it's range
  if(fd >= 130 && fd <= 1){
    return -1;
  }

  struct file *file = thread_get_file(fd);
  if(file == NULL) return -1;

  // File_writes SIZE bytes from BUFFER into FILE
  int written_bytes = file_write(file, buffer2, size);
  if(written_bytes == 0) return -1;
  return written_bytes;
}

void exit(int status){
    thread_close_all_files();
    thread_exit ();
}
