#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#ifndef _PROC_FILE_H_
#define _PROC_FILE_H_

struct slog
{
	uid_t uid;
	time_t stime;
	char message[100];
};

int proc_read_log(char *page, char **start, off_t off, int count, int *eof, void *data);
int proc_write_log(struct slog _log);
int proc_init(void);
void proc_exit(void);

#endif