#include "proc_file.h"

static struct slog logs[1024];

//static char values[4096];
static int read_index = -1;
static int write_index = 0;

static struct proc_dir_entry *proc_file;

int proc_read_log(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;

	read_index = (read_index + 1) % 1024;

	if (read_index == write_index)
	{
		read_index = read_index - 1;
		return -1;
	}

	struct slog alog = logs[read_index];

	char strlog[151];
	memset(strlog, '-', 151);
	strlog[150] = '\0';
	sprintf(strlog, "User:%d-------Time:%d------------------Message : %s", alog.uid, alog.stime, alog.message);
	strlog[strlen(strlog)] = '-';

	len = sprintf(page, strlog);

	printk(KERN_ALERT "Proc_read_log -- Slog info : uid - %d, stime - %d, message - %s Read_index - %d Write_index - %d\n", alog.uid, alog.stime, alog.message, read_index, write_index);
	return len;
}

int proc_write_log(struct slog _log)
{

	if(write_index == read_index)
	{
		return -1;
	}

	logs[write_index].uid = _log.uid;
	logs[write_index].stime = _log.stime;
	strcpy(logs[write_index].message, _log.message);

	write_index = (write_index + 1) % 1024;

	//printk(KERN_ALERT "The values is %s, length is %d final is %d", buffer, strlen(buffer), buffer[12]);
	printk(KERN_ALERT "Proc_write_log -- Slog info : uid - %d stime - %d, message - %s Read_index - %d Write_index - %d\n", _log.uid, _log.stime, _log.message, read_index, write_index);
	return 0;
}

int proc_init(void)
{
	proc_file = create_proc_entry("proc_log", 0644, NULL);
	if (proc_file == NULL)
	{
		remove_proc_entry("proc_log", NULL);
		return -1;
	}
	//proc_file->data = values;
	proc_file->read_proc = proc_read_log;
	//proc_file->write_proc = proc_write_log;

	printk(KERN_ALERT "Proc -- Proc file init");
	return 0;
}

void proc_exit(void)
{
	remove_proc_entry("proc_log", NULL);
	printk(KERN_ALERT "Proc -- Proc file exit");
}