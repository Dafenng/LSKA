#include <stdio.h>
#include <fcntl.h>
#include <pwd.h> 
#include <stdlib.h> 
#include <malloc.h>
#include <time.h>

int main(int argc, char const *argv[])
{
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

		printf("%s\n", buffer);
	}
	else
	{
		printf("Empty\n");
	}
	close(fp);
	return 0;
}