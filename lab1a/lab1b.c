
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>  //might not need this

void errorandexit(char *errmsg, struct termios attrscp);
void restore(struct termios attrscp);

int main(int argc, char* argv[])
{
int fd0 = 0, fd1 = 1, opt = -1, attr = 0, pipe1 = 0, pipe2 = 0;
struct termios terminalAttrs, attrsCopy;
ssize_t bytes_read = 0, bytes_written = 0;
char buf[256];
int long_index = 0;
char* program = "/bin/bash";
int shellflag = 0; 
pid_t pid;
int pipefd1[2], pipefd2[2];
struct pollfd pollfds[2];

attr = tcgetattr(0, &terminalAttrs);
if (attr == -1)
{
	fprintf(stderr, "Couldn't save current terminal modes\n");
	restore(attrsCopy);
}

attrsCopy = terminalAttrs;
terminalAttrs.c_iflag = ISTRIP;		/* only lower 7 bits	*/
terminalAttrs.c_oflag = 0;		/* no processing	*/
terminalAttrs.c_lflag = 0;		/* no processing	*/

if (tcsetattr(0, TCSANOW, &terminalAttrs) < 0)
	errorandexit("tcsetattr failed\n", attrsCopy);

static struct option long_options[] =
{
	{"shell=", optional_argument, NULL, 's'}
};

while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
{
	switch (opt)
	{
		case 's':
			program = optarg;
			shellflag = 1;
			break;
		default:
			errorandexit("\rUnrecognized option entered\r\n", attrsCopy);
	}
}

if (shellflag == 0)
{
while ((bytes_read = read (fd0, buf, 256)))
{
	int i;
	for (i = 0; i < bytes_read; i++)
	{
		if (buf[i] == 0x0A || buf[i] == 0x0D)
			printf("\r\n");
		if (buf[i] == 0x04)
			restore(attrsCopy);
		bytes_written = write(fd1, buf, 1);
		if (bytes_written == -1)
			errorandexit("Couldn't write to stdout\n", attrsCopy);
	}

}
if (bytes_read == -1)
	errorandexit("Couldn't read from stdin\n", attrsCopy);
}


else  //shell option specified
{
int ret = 0;
pipe1 = pipe(pipefd1);
pipe2 = pipe(pipefd2);
if (pipe1 == -1 || pipe2 == -1)
	errorandexit("Couldn't create pipe/s\n", attrsCopy);

pid = fork();
int eofreceivedin = 0;
if (pid != 0)  //parent process
{
	close(pipefd1[0]);
	close(pipefd2[1]);
	pollfds[0].fd = fd0;
	pollfds[1].fd = pipefd2[0];	
	pollfds[0].events = POLLIN | POLLHUP | POLLERR;
	pollfds[1].events = POLLIN | POLLHUP | POLLERR;	
while(1)
{
	bytes_read = read(fd0, buf, 256);
	if (bytes_read == -1)
		errorandexit("Couldn't read from stdin\n", attrsCopy);
	int i;
	for (i = 0; i < bytes_read; i++)
	{
		if (buf[i] == 0x0A || buf[i] == 0x0D)
		{
			char cr = 0x0D;
			char lf = 0x0A;
			write(1, &cr, 1);
			write(1, &lf, 1);
			write(pipefd1[1], &lf, 1);
		}
		else if (buf[i] == 0x03)
			kill(pid, SIGINT); 
		else if (buf[i] == 0x04)
		{
			close (pipefd1[1]);
			eofreceivedin = 1;
			break;
		}
		else
		{
		bytes_written = write(fd1, &buf[i], 1);  //echo to stdout
		ssize_t bytes_writtensh = write(pipefd1[1], &buf[i], 1);
		if ((bytes_written == -1) || (bytes_writtensh == -1)) //w to shell
			errorandexit("Couldn't write to stdout/shell\n", attrsCopy);
		}
	}

	ret = poll(pollfds, 2, -1);	
	if (ret == -1)
		errorandexit("Failed to poll\n", attrsCopy);
	if (pollfds[1].revents & POLLIN)  //input from shell received
	{
		bytes_read = read(pipefd2[0], buf, 256);
		if (bytes_read == -1)
			errorandexit("Couldn't read from stdin\n", attrsCopy);
		
		int i;
		for (i = 0; i < bytes_read; i++)
		{
			if (buf[i] == 0x04)
				eofreceivedin = 1;
			if (buf[i] == 0x0A)
			{
				char cr = 0x0D;
				write(1, &cr, 1);
			}
			bytes_written = write(fd1, &buf[i], 1);
			if (bytes_written == -1)
				errorandexit("Couldn't write to stdout\n", attrsCopy);
		}			
	}
/*
	else if (pollfds[1].revents & (POLLHUP || POLLERR))
	{
	int stats = 0;
	waitpid(0, &stats, WUNTRACED);	
	}
*/
	else if (eofreceivedin ||(pollfds[1].revents & (POLLHUP || POLLERR)) )
	{
		int stats = 0;
		int SIG = 0, STAT = 0;
		close(pipefd2[0]);
		waitpid(0, &stats, WUNTRACED);
		SIG = stats & 0x007f;
		STAT = stats & 0xff00;
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", SIG, STAT); 
		restore(attrsCopy);
	}
}
}
else	//child process
{
	close(pipefd1[1]);
	close(pipefd2[0]);
	dup2(pipefd1[0], fd0);
	dup2(pipefd2[1], fd1);
	dup2(pipefd2[1], 2);
	execl(program, "sh", (char *) NULL);	
	close(pipefd1[0]);
	close(pipefd2[1]);	
}
}	
tcsetattr(0, TCSANOW, &attrsCopy);	//might need to put this elsewhere
return 0;	
}

void restore(struct termios attrscp)
{
tcsetattr(0, TCSANOW, &attrscp);
exit(0);
}

void errorandexit(char *errmsg, struct termios attrscp)
{
	fprintf(stderr, errmsg);
	restore(attrscp);
}
