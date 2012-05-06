#include <stdio.h> 
#include <pwd.h> 
#include <stdlib.h> 
#include <malloc.h> 

int main(void) 
{ 

	char *login_name = (char *)malloc(32 * sizeof(char));

	struct passwd *pwd;

	pwd = getpwuid(1000);

	login_name = pwd->pw_name;

	printf("当前登陆的用户名为：%s\n", login_name);

	free(login_name);

	return EXIT_SUCCESS;

}