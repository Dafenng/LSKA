#include "stdio.h"
#include "time.h"

int main(int argc, char const *argv[])
{
	time_t timep = 1336035076;
	//time(&timep);
	printf("%s", ctime(&timep));
	return 0;
}