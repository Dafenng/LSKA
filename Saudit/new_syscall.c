#include <linux/sched.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/time.h>

#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/limits.h>

#include "linux/fs.h"
#include "linux/file.h"
#include "linux/fdtable.h"
#include "linux/dcache.h"
#include "util.h"

#include "new_syscall.h"
#include "kernel_file_op.h"
#include "proc_file.h"

#define MAX_DIR_LENTH 100

void test2(unsigned int fd)
{
    struct file *file = NULL;
    char* fullpath = NULL;

    //open /tmp/123
    file = fget(fd);
    if (IS_ERR(file)) {
        printk( "error occured while opening file, exiting...");
        return;
    }

    //get the fullpath
    fullpath = getfullpath2(file);
    if(!fullpath){
        printk("Get fullpath error!");
        return;
    }
    printk("FULLPATH:%s",fullpath);

    //free mem
    putfullpath(fullpath);

    //close /tmp/123
    if(file != NULL)
    filp_close(file, NULL);

}

asmlinkage int (*original_read)(unsigned int, const char __user *, size_t);

asmlinkage int new_read(unsigned int fd, const char __user *buf, size_t count) 
{

    // hijacked write
    char *name = "cat";
    if (strcmp(current->comm, name) == 0)
    {
        printk(KERN_ALERT "HIAJCK -- write hiajcked and process is %s\n", current->comm);
        struct timeval tv;
        do_gettimeofday(&tv);

        test2(fd);

        struct slog alog = {current->real_cred->uid, tv.tv_sec, "fuck"};

        printk(KERN_ALERT "HIJACK -- Slog info : uid - %d, stime - %d, message - %s", alog.uid, alog.stime, alog.message);
        //struct file *fp;
        //fp = file_open("/dev/scullp0", O_RDWR, 0644);
        //file_write(fp, 0, (char *)&alog, sizeof(struct slog));
        //file_close(fp);
        proc_write_log(alog);

        /*
        struct timex  txc;
        struct rtc_time tm;
        do_gettimeofday(&(txc.time));
        rtc_time_to_tm(txc.time.tv_sec,&tm);
        printk(KERN_ALERT "UTC time :%d-%d-%d %d:%d:%d \n",tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        */   
    }

    return (*original_read)(fd, buf, count);
}

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

        //test2(fd);

        struct slog alog = {current->real_cred->uid, tv.tv_sec, "fuck"};

        printk(KERN_ALERT "HIJACK -- Slog info : uid - %d, stime - %d, message - %s", alog.uid, alog.stime, alog.message);
        //struct file *fp;
        //fp = file_open("/dev/scullp0", O_RDWR, 0644);
        //file_write(fp, 0, (char *)&alog, sizeof(struct slog));
        //file_close(fp);
        proc_write_log(alog);

        /*
        struct timex  txc;
        struct rtc_time tm;
        do_gettimeofday(&(txc.time));
        rtc_time_to_tm(txc.time.tv_sec,&tm);
        printk(KERN_ALERT "UTC time :%d-%d-%d %d:%d:%d \n",tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        */   
    }

    return (*original_write)(fd, buf, count);
}

asmlinkage long (*original_open)(const char __user *, int, int);

asmlinkage long new_open(const char __user *filename, int flags, int mode)
{

	return (*original_open)(filename, flags, mode);
}