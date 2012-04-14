#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> 
#include <signal.h>

FILE *fp = NULL;

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
		fprintf(fp, "Terminate logging...\n");
		fflush(fp);
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
	fp = fopen("Log.txt", "w+");

	signal(SIGTERM, sig_term);

	while(1)
	{
		sleep(10);
		fprintf(fp, "Logging info...\n");
		fflush(fp);
	} 

	fprintf(fp, "Finish logging...\n");
	fflush(fp);
	fclose(fp);
	return 0;
}