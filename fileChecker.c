#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_EVENT_MONITOR 2048
#define NAME_LEN 32
#define MONITOR_EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN MAX_EVENT_MONITOR*(MONITOR_EVENT_SIZE+NAME_LEN)

int main(){
	int fd, watch_desc;
	char buffer[BUFFER_LEN];
	fd = inotify_init();

	char *filePath = "/home/vatsav14/code/test/";
	struct stat t_stat;

	if (fd < 0)
		printf("Notify did not initialize");

	watch_desc = inotify_add_watch(fd, "/home/vatsav14/code/test", IN_CREATE);

	if (watch_desc == -1)
		printf("Couldn't add watch to the path");
	else
		printf("Monitoring path...\n");
	
	while(1){
		int total_read = read(fd, buffer, BUFFER_LEN);
		if(total_read < 0)
			printf("Read error");

		int i = 0;	
		while(i < total_read){
			struct inotify_event *event = (struct inotify_event*) &buffer[i];
			if(event->len){
				if(event->mask & IN_CREATE){

					// Get the date-time of file creation
					char fileName[256];
					snprintf(fileName, sizeof(fileName), "%s%s", filePath, event->name);
					// To print out the return value of stat: printf("%d\n", stat(fileName, &t_stat));
					stat(fileName, &t_stat);
					struct tm *timeinfo = localtime(&t_stat.st_ctime);

					if(event->mask & IN_ISDIR)
						printf("Directory \"%s\" was created\n", event->name);
					else
						printf("File \"%s\" was created on %s\n", event->name, asctime(timeinfo));
				}
				i += MONITOR_EVENT_SIZE + event->len;
			}
		}
	}

	inotify_rm_watch(fd, watch_desc);
	close(fd);
}
