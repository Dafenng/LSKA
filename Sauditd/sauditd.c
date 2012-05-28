#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> 
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>  
#include <malloc.h>
#include <time.h>

FILE *fp2 = NULL;

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
	fp2 = fopen("/home/keywind/proc_log.txt", "w+");

	signal(SIGTERM, sig_term);

	while(1)
	{
		sleep(10);
		
		int fp = open("/proc/proc_log", O_RDONLY);
		char buffer[150] = {0};
		read(fp, buffer, 150);

		if (buffer[0] != 0)
		{
			int i, j, user, _time;
			_time = user = 0;
			for(i = 5; isdigit(buffer[i]); i++) {
				user = user * 10 + buffer[i] - '0';
			}

			for(i = 21; isdigit(buffer[i]); i++) {
				_time = _time * 10 + buffer[i] - '0';
			}

			char *login_name;
			struct passwd *pwd;
			pwd = getpwuid(user);
			login_name = pwd->pw_name;

			char *occur_time;
			occur_time = ctime(&_time);

			for (i = 0, j = 5; login_name[i]; i++, j++) {
				buffer[j] = login_name[i];
			}

			for (i = 0, j = 21; occur_time[i]!='\n'; i++, j++) {
				buffer[j] = occur_time[i];
			}

			fprintf(fp2, "%s\n", buffer);
			fflush(fp2);
		}
		close(fp);		
	} 

	fprintf(fp2, "Finish logging...\n");
	fflush(fp2);
	fclose(fp2);
	return 0;
}