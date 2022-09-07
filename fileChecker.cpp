#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>
#include <time.h>
#include <boost/program_options.hpp>

#define MAX_EVENT_MONITOR 2048
#define NAME_LEN 32
#define MONITOR_EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN MAX_EVENT_MONITOR*(MONITOR_EVENT_SIZE+NAME_LEN)

using namespace std;
namespace po = boost::program_options;

void outputFunc(int, string, fstream&);

int main(int ac, char* av[]){
	int fd, watch_desc, out;
	char buffer[BUFFER_LEN];
	char outputMsg[300];
	fd = inotify_init();
	string path;

	// Boost command line options
	try {
		po::options_description desc("Allowed options");
		desc.add_options()("help,h", "Show this help message")(
			"path",
			po::value<string>(&path)->default_value("/data/cat/LangLab/dev/file_checker"),
			"Set the path to be monitored")(
			"out",
			po::value<int>(&out)->default_value(0),
			"0 for stdout, 1 for file, 2 for mqtt");

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << endl;
			return 0;
		}
	} catch (exception const& e) {
		cerr << "error: " << e.what() << endl;
		return 1;
	} catch (...) {
		cerr << "Exception of unknown type!" << endl;
	}
	
	char filePath[128]; // = "/data/cat/LangLab/dev/file_checker";
	strcpy(filePath, path.c_str());
	struct stat t_stat;
	struct timeval tv;
	fstream outFile;
	outFile.open("outFile.txt", ios::out); // Change to ios::app to append instead

	if (fd < 0)
		printf("Notify did not initialize");

	watch_desc = inotify_add_watch(fd, filePath, IN_CREATE);

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

					// Get the timeval data
					gettimeofday(&tv, NULL);

					// Get the date-time of file creation
					char fileName[256];

					// Format the string to get the complete path of the file
					snprintf(fileName, sizeof(fileName), "%s/%s", filePath, event->name);
					// To print out the return value of stat: printf("%d\n", stat(fileName, &t_stat));
					stat(fileName, &t_stat);
					struct tm *timeinfo = localtime(&t_stat.st_ctime);

					if(event->mask & IN_ISDIR)
						printf("Directory \"%s\" was created\n", event->name);
					else {
						//printf("%s: %ld.%09ld\n", event->name, t_stat.st_ctim.tv_sec, t_stat.st_ctim.tv_nsec);
						//printf("%s: %ld.%06ld\n", event->name, tv.tv_sec, tv.tv_usec);

						// Format the output string
						snprintf(outputMsg, sizeof(outputMsg), "%s: %ld.%09ld\n", event->name, t_stat.st_ctim.tv_sec, t_stat.st_ctim.tv_nsec);
						
						cout << out;
						outputFunc(out, outputMsg, outFile);
					}
				}
				i += MONITOR_EVENT_SIZE + event->len;
			}
		}
	}
	
	outFile.close();
	inotify_rm_watch(fd, watch_desc);
	close(fd);
}

// Function to output the timestamp in the format specified by the
// boost argument '--out'
void outputFunc(int choice, string output, fstream &outFile) {
	switch(choice) {
		case 0: cout << output;
			break;

		case 1: if (outFile.is_open()) {
				outFile << output;
			}
			break;

		case 2: printf("This is where the code for mosquitto goes");
			// NOTES:
			// -- The string to be used as output is 'output'
			// -- No need to format anything else
			break;
	}
}
