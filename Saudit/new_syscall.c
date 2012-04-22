#include <linux/sched.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/time.h>

#include "new_syscall.h"
#include "kernel_file_op.h"
#include "proc_file.h"

asmlinkage int (*original_write)(unsigned int, const char __user *, size_t);

asmlinkage int new_write(unsigned int fd, const char __user *buf, size_t count) 
{

    // hijacked write
    char *name = "cat";
    if (strcmp(current->comm, name) == 0)
    {
        printk(KERN_ALERT "HIAJCK -- write hiajcked and process is %s\n", current->comm);
        struct timeval tv;
        do_gettimeofday(&tv);
        struct slog alog = {37, tv.tv_sec, "Someone is writing something at somtimes"};
        printk(KERN_ALERT "HIJACK -- Slog info : uid - %d, stime - %d, message - %s", alog.uid, alog.stime, alog.message);
        //struct file *fp;
        //fp = file_open("/dev/scullp0", O_RDWR, 0644);
        //file_write(fp, 0, (char *)&alog, sizeof(struct slog));
        //file_close(fp);
        proc_write_log(NULL, NULL, sizeof(struct slog), &alog);
    }

    return (*original_write)(fd, buf, count);
}

asmlinkage long (*original_open)(const char __user *, int, int);

asmlinkage long new_open(const char __user *filename, int flags, int mode)
{

	return (*original_open)(filename, flags, mode);
}