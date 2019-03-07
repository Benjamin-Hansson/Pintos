#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/init.h"
#include "stdbool.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"
#include "process.h"
#include "pagedir.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  struct thread *t= thread_current();
  validate_ptr(f->esp, t);
  int *pointer = f->esp;
  int sys_call_number = *pointer;

  //variable declarations
  void *buffer;
  unsigned size;
  char *file;
  unsigned initial_size;
  int fd;
  int exit_status;
  pid_t pid;
  unsigned position;

  switch(sys_call_number){
    case(SYS_HALT):
      halt();
      break;

    case(SYS_EXIT):
      validate_ptr(pointer+1, t);
      exit_status = (int) *(pointer+1);
      exit(exit_status);
      break;

    case(SYS_EXEC):
      validate_ptr(pointer+1, t);
      validate_char((char*) *(pointer+1));
      file = (char*) *(pointer+1);
      f->eax = exec(file);
      break;

    case(SYS_WAIT):
      validate_ptr(pointer+1, t);
      pid = (pid_t) *(pointer+1);
      f->eax = wait(pid);
      break;

    case(SYS_CREATE):
      validate_ptr(pointer+1, t);
      validate_ptr(pointer+2, t);

      validate_char((char*) *(pointer+1));

      file = (char*) *(pointer+1);
      initial_size = (unsigned) *(pointer+2);
      f->eax = create(file, initial_size);
      break;

    case(SYS_REMOVE):
      validate_ptr(pointer+1, t);
      validate_char((char*) *(pointer+1));
      file = (char*) *(pointer+1);
      f->eax = remove(file);
      break;

    case(SYS_OPEN):
      validate_ptr(pointer+1, t);

      validate_char((char*) *(pointer+1));
      file = (char*) *(pointer+1);
      f->eax = open(file);
      break;

    case(SYS_FILESIZE):
      validate_ptr(pointer+1, t);
      fd = *(pointer+1);
      f->eax = file_size(fd);
      break;

    case(SYS_READ):
      validate_ptr(pointer+1, t);
      validate_ptr(pointer+2, t);
      validate_ptr(pointer+3, t);

      fd = *(pointer+1);
      buffer = (void*)(pointer+2);
      size = (unsigned) *(pointer +3);
      validate_buffer(buffer, size);
      f->eax = read(fd, buffer, size);
      break;

    case(SYS_WRITE):
      validate_ptr(pointer+1, t);
      validate_ptr(pointer+2, t);
      validate_ptr(pointer+3, t);
      fd = *(pointer+1);
      buffer = (char*)(pointer+2);
      size = (unsigned) *(pointer +3);
      validate_buffer(buffer, size);
      f->eax = write(fd, buffer, size);
      break;

    case(SYS_SEEK):
      validate_ptr(pointer+1, t);
      validate_ptr(pointer+2, t);
      fd = *(pointer+1);
      position = (unsigned) *(pointer+2);
      seek(fd, position);
      break;

    case(SYS_TELL):
      validate_ptr(pointer+1, t);
      fd = *(pointer+1);
      f->eax = tell(fd);
      break;

    case(SYS_CLOSE):
      validate_ptr(pointer+1, t);
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
  return;



}
/*
* validates a char pointer
*/
void validate_char(char *c) {
  struct thread *t = thread_current();
  validate_ptr(c, t);
  while (*c != '\0') {
    c++;
    validate_ptr(c, t);
  }
}
/*
* assumes that buffer is safe to dereference
*/
void validate_buffer(void *buffer, unsigned size){
  void **buffer2 = (void**) buffer;
  struct thread *t = thread_current();
  for(unsigned i = 0; i < size; i+=1){
    validate_ptr(*buffer2+i, t);
  }
}


// ptr can be dereferenced after passing this test
void validate_ptr(void *f, struct thread *t){
  if(!is_user_vaddr(f) ||
  pagedir_get_page(t->pagedir, f) == NULL) exit(PTR_ERROR);
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

// Reads (size) bytes from keyboard
int read_from_console(void* buffer, unsigned size){

  for(unsigned i = 0; i < size; i++){
    *((uint8_t *)buffer) = input_getc();
    buffer++;
  }
  return size;

}

int read (int fd, void *buffer, unsigned size){
  void *buffer2 =  *((void**)buffer); //LIKE A BOSS
  // If 0, then read input from keyboard
  if(fd == 0){
    return read_from_console(buffer2, size);
  }
  // if fd is outside it's range
  if(fd >= 130 || fd < 2){
    return -1;
  }
  else{
    struct file *file = thread_get_file(fd);
    if(file == NULL) return -1;
    // file_read reads SIZE bytes from FILE into BUFFER
    int read_bytes = file_read(file, buffer2, size);
    return read_bytes;
  }
}


int write(int fd, const char *buffer, unsigned size){
  const char *buffer2 =  *((const char**)buffer);
  if(fd == 1){
    putbuf(buffer2, (size_t)size);
    return size;
  }

  // if fd is outside it's range
  if(fd >= 130 || fd < 2){
    return -1;
  }

  struct file *file = thread_get_file(fd);
  if(file == NULL) return -1;


  // File_writes SIZE bytes from BUFFER into FILE
  int written_bytes = file_write(file, buffer2, size);



  return written_bytes;
}

void exit(int exit_status){
    printf("%s: exit(%d)\n", thread_current()->name, exit_status);
    thread_save_exit_status(exit_status);
    thread_exit();
}

tid_t exec(const char *file){
  if(file == NULL) return -1;
  return process_execute(file);
}

int wait(pid_t pid) {
  return process_wait(pid);
}

bool remove(const char *file_name){
  return filesys_remove(file_name);
}

void seek(int fd, unsigned position){
  // if fd is outside it's range
  if(fd >= 130 || fd < 2) return -1;

  struct file *file = thread_get_file(fd);
  if (file == NULL) return;
  if (position > (unsigned)file_length(file)) position = file_length(file); //TODO: KANSKE SKA VARA +-1 //If position exceeds file size set to end of file
  file_seek(file, position);
}

int file_size (int fd){
  // if fd is outside it's range
  if(fd >= 130 || fd < 2) return -1;

  struct file *file = thread_get_file(fd);
  if (file == NULL) return -1; // TODO: FRÅGA ANNA!!!!
  return file_length(file); //TODO: KANSKE SKA VARA +-1
}

unsigned tell (int fd){
  // if fd is outside it's range
  if(fd >= 130 || fd < 2) return -1;


  struct file *file = thread_get_file(fd);
  if (file == NULL) return -1;
  return file_tell(file); //TODO: KANSKE SKA VARA +-1
}
