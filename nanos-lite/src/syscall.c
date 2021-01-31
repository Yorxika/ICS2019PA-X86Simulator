#include "common.h"
#include "syscall.h"
#include "fs.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:
      _yield();
      c -> GPRx = 0;
      break;
    case SYS_exit:
      //_halt(0);
      naive_uload(NULL, "/bin/init");
      c -> GPRx = 0;
      break;
    case SYS_write:
      //printf("call fs_write at %d ,addr: %p ,len: %d\n", a[1], a[2], a[3]);
      c -> GPRx = fs_write(a[1], (void *)a[2], a[3]);
      break;
    case SYS_brk:
      //printf("call SYS_brk , always return 0.\n");
      c -> GPRx = 0;
      break;
    case SYS_read:
    	c -> GPRx = fs_read(a[1], (void *)a[2], a[3]);
    	break;
    case SYS_open:
    	c -> GPRx = fs_open((char *)a[1], a[2], a[3]);
    	break;
    case SYS_close:
    	c -> GPRx = fs_close(a[1]);
    	break;
    case SYS_lseek:
    	c -> GPRx = fs_lseek(a[1], a[2], a[3]);
    	break;
    case SYS_execve:
      //printf("a[1]: %s\n",a[1]);
      naive_uload(NULL, (const char *)a[1]);
      break;
    default: 
      panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
