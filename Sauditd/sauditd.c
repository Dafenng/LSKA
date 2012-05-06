#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> 
#include <signal.h>
#include <fcntl.h>

int fp;
FILE *fp2 = NULL;

struct slog
{
	uid_t uid;
	time_t stime;
	char message[100];
};

int daemon_init()
{
	pid_t process_id = 0;
	pid_t sid = 0;

	//Create child process
	process_id = fork();
	if (process_id < 0)
		return -1;

	//Kill parent process
	if (process_id > 0)
		exit(0);

	//Unmask the file mode
	umask(0);

	//Set new session
	sid = setsid();
	if (sid < 0)
	{
		exit(-1);
	}

	//Change the current working directory to root
	chdir("/tmp/");

	//Close stdin, stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	return 0;
}

void sig_term(int signo)
{
	if (signo == SIGTERM)
	{
		fprintf(fp2, "Terminate logging...\n");
		fflush(fp2);
		fclose(fp2);
		close(fp);
		exit(0);
	}
}

int main(int argc, char const *argv[])
{	
	if (daemon_init() == -1)
	{
		printf("Fork failed\n");
		exit(0);
	}

	/*
	 *TODO:Open a log file in write mode for test
	 *Need to be my chracter device
	 */
	fp = open("/proc/proc_log", O_WRONLY, O_CREAT);
	fp2 = fopen("/home/keywind/proc_log.txt", "w+");

	signal(SIGTERM, sig_term);

	struct slog alog;
	while(1)
	{
		sleep(10);
		
		if (fp != -1)
		{
			read(fp, &alog, sizeof(struct slog));
			fprintf(fp2, "Slog info : uid - %d, stime - %d, message - %s\n", alog.uid, alog.stime, alog.message);
		}
		else
		{
			fprintf(fp2, "Proc not working\n");
		}

		
		fprintf(fp2, "Just test line\n");
		fflush(fp2);
	} 

	fprintf(fp2, "Finish logging...\n");
	fflush(fp2);
	fclose(fp2);
	close(fp);
	return 0;
}