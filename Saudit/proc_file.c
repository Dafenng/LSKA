#include "proc_file.h"

static struct slog logs[1024];

static int read_index = 0;
static int write_index = 0;

static struct proc_dir_entry *proc_file;

int proc_read_log(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;

	if (read_index == write_index)
	{
		return -1;
	}

	if (len = copy_to_user(page, (char *)&logs[read_index], sizeof(struct slog)))
	{
		return -1;
	}

	struct slog alog = logs[read_index];

	read_index = (read_index + 1) % 1024;

	printk(KERN_ALERT "Proc -- Slog info : uid - %d, stime - %d, message - %s Read_index - %d", alog.uid, alog.stime, alog.message, read_index);
	return len;
}

int proc_write_log(struct file *file, const char *buffer, unsigned long count, void *data)
{
	struct slog *fb_data = (struct slog *)data;

	struct slog alog = logs[write_index];
	alog.uid = fb_data->uid;
	alog.stime = fb_data->stime;
	strcpy(alog.message, fb_data->message); 
	write_index = (write_index + 1) % 1024;
	printk(KERN_ALERT "Proc -- Slog info : uid - %d, stime - %d, message - %s Write_index - %d", alog.uid, alog.stime, alog.message, write_index);
	return count;
}

int proc_init(void)
{
	proc_file = create_proc_entry("proc_log", 0644, NULL);
	if (proc_file == NULL)
	{
		return -1;
	}
	proc_file->read_proc = proc_read_log;
	proc_file->write_proc = proc_write_log;

	printk(KERN_ALERT "Proc -- Proc file init");
	return 0;
}

void proc_exit(void)
{
	remove_proc_entry("proc_log", NULL);
	printk(KERN_ALERT "Proc -- Proc file exit");
}