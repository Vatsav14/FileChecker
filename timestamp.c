#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

int main() {
	//struct stat t_stat;
	//stat("haha", &t_stat);
	//struct tm *timeinfo = localtime(&t_stat.st_ctime);
	//printf("File time and date: %s", asctime(timeinfo));
	
	struct tm* clock;
	struct stat attrib;
	printf("%d", stat("haha", &attrib));
	clock = localtime(&(attrib.st_ctime));
	printf("%s", asctime(clock));

	return 0;
}
