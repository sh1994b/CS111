
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <mraa.h>
#include <poll.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>

int period = 1;
char scale = 'F';
int logflag = 0, logfd, ret;
char *logfile;
struct timeval tv, temptv;
struct tm *localTime;
mraa_aio_context temp;
mraa_gpio_context button;
char buf[32];
ssize_t bytes_read;
int doNotReport = 0;
			
void errexit(char* msg);
void printTime();
void getCommands();
void shutdown();
void printTemp();

int main(int argc, char* argv[])
{

	int long_index = 0;
	int opt = -1;
	static struct option long_options[] =
        {
                {"period=", required_argument, NULL, 'p'},
		{"scale=", required_argument, NULL, 's'},
		{"log=", required_argument, NULL, 'l'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
        {
                switch (opt)
                {
			case 'p':
				period = atoi(optarg);
				break;
			case 's':
				scale = *optarg;
				if (scale != 'F' && scale != 'C')
					errexit("Non-supported scale\n");
				break;
			case 'l':
				logflag = 1;
				logfile = optarg;
				break;
			default:
				errexit("Unrecognized option entered\n");
		}
	} 
	
	if (logflag)
	{
		logfd = open(logfile, O_CREAT | O_WRONLY, 0644);
		if (logfd == -1)
			errexit("Couldn't open log file\n");
		if(access(logfile, W_OK) == -1)
			errexit("log file not writable\n");
	}

	temp = mraa_aio_init(1);
	if (temp == NULL)
		errexit("Couldn't initialize temp sensor\n");
	
	button = mraa_gpio_init(60);
	if (button == NULL)
		errexit("Couldn't initialize button\n");
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &shutdown, NULL);

	struct pollfd pollfds[1];
	pollfds[0].fd = 0;
	pollfds[0].events = POLLIN;
	while (1)
	{
		printTime();
		ret = poll(pollfds, 1, 0);
		if (ret == -1)
			errexit("poll() failed\n");
		if (pollfds[0].revents & POLLIN) //input received
		{
			memset(buf,0,strlen(buf));
			bytes_read = read(0, buf, 32);
			getCommands();
		}
	}	

	mraa_aio_close(temp);
	mraa_gpio_close(button);	
	if (logflag)
		close(logfd);
	return 0;
}

void printTime()
{
	if (doNotReport)
		return;
	gettimeofday(&tv, NULL);
	if (tv.tv_sec - temptv.tv_sec >= period)
	{
		localTime = localtime(&tv.tv_sec);
		printf("%02d:%02d:%02d ", localTime->tm_hour,\
		 localTime->tm_min, localTime->tm_sec);
		if (logflag)
		{
			dprintf(logfd, "%02d:%02d:%02d ", localTime->tm_hour,\
			 localTime->tm_min, localTime->tm_sec);
		}
		printTemp();
		temptv.tv_sec = tv.tv_sec;
	}
}

void printTemp()
{
	const int B = 4275;               // B value of the thermistor
	const int R0 = 100000;            // R0 = 100k
	int a = mraa_aio_read(temp);
	float R = 1023.0/a-1.0;
	R = R0*R;
	float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;
	if (scale == 'F')
		temperature = temperature*9/5 + 32;
	printf("%.1f\n", temperature);
	if (logflag)
		dprintf(logfd, "%.1f\n", temperature);
}

void getCommands()
{
	char *command;
	command = malloc((bytes_read) * sizeof(char));
	memset(command, 0, strlen(command));
	int i;
	for (i = 0; i < bytes_read-1; i++)
		command[i] = buf[i];
	command[bytes_read-1] = '\0';

	if (strcmp(command, "SCALE=F") == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
		scale = 'F';
	}
	else if (strcmp(command, "SCALE=C") == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
		scale = 'C';
	}
	else if (strncmp(command, "PERIOD=", 7) == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
		char *tempptr = command;
		while (*tempptr)
		{
			if (*tempptr++ == '=')
				period = atoi(tempptr);
		}
	}
	else if (strcmp(command, "STOP") == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
		doNotReport = 1;
	}
	else if (strcmp(command, "START") == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
		doNotReport = 0;
	}	
	else if (strcmp(command, "OFF") == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
		shutdown();
	}		
	else if (strncmp(command, "LOG=", 4) == 0)
	{
		if (logflag)
			dprintf(logfd, "%s\n", command);
	}

	free(command);
	return;
}

void shutdown()
{
	localTime = localtime(&tv.tv_sec);
	printf("%02d:%02d:%02d ", localTime->tm_hour,\
	localTime->tm_min, localTime->tm_sec);
	if (logflag)
	{
		dprintf(logfd, "%02d:%02d:%02d ", localTime->tm_hour,\
		 localTime->tm_min, localTime->tm_sec);
	}	
	printf("SHUTDOWN\n");
	if (logflag)
		dprintf(logfd, "SHUTDOWN\n");
	exit(0);
}

void errexit(char* msg)
{
        fprintf(stderr, msg);
        exit(1);
}

