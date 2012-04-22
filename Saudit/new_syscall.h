#ifndef _NEW_SYSCALL_H_
#define _NEW_SYSCALL_H_

asmlinkage int new_write(unsigned int fd, const char __user *buf, size_t count);

#endif