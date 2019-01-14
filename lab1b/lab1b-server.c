
#include <mcrypt.h>
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

void errexit(char* errmsg);
void closeenc(MCRYPT sockfd);


int main(int argc, char* argv[])
{
	int fd0 = 0, fd1 = 1, opt = -1, sockfd = 0, connected_sockfd = 0; 
	int pipe1 = 0, pipe2 = 0;
	ssize_t bytes_read = 0, bytes_written = 0;
	char buf[256];
	int long_index = 0;
	char* program = "/bin/bash";
	pid_t pid;
	int pipefd1[2], pipefd2[2];
	struct pollfd pollfds[2];
	int encflag = 0;
	char* keyfile = "my.key";
	int port = -1;	
	char keybuf[10];
	MCRYPT mcryptfd, mdecryptfd;
	int stats = 0, SIG = 0, STAT = 0;
        if (argc < 2)
        {
                fprintf(stderr, "Please enter port number as an argument\n");
                exit(1);
        }


	static struct option long_options[] =
	{
		{"port=", required_argument, NULL, 'p'},
		{"encrypt=", optional_argument, NULL, 'e'}
	};

	while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
	{
		switch (opt)
		{
			case 'p':
				port = atoi(optarg);
				break;
			case 'e':
				keyfile = optarg;
				encflag = 1;
				break;
			default:
				errexit("Unrecognized option entered\n");
		}
	}

	if (encflag)
        {
                char *IV;
                int i;
                mcryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
                mdecryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
                if (mcryptfd == MCRYPT_FAILED)
                        errexit("\rmcrypt_module_open failed for mcrypt\r\n");
                if (mdecryptfd == MCRYPT_FAILED)
                        errexit("\rmcrypt_module_open failed for mdecrypt\r\n");
                int keyfd = open(keyfile, O_RDONLY, 0444);
                if (keyfd == -1)
                        errexit("\rCouldn't open key file\r\n");
                bytes_read = read (keyfd, keybuf, sizeof(keybuf));
                IV = malloc(mcrypt_enc_get_iv_size(mcryptfd));
                for (i=0; i< mcrypt_enc_get_iv_size(mcryptfd); i++)
                        IV[i]=i;
                if (mcrypt_generic_init (mcryptfd, keybuf, sizeof(keybuf), IV) < 0)
                        errexit("\rmcrypt_generic_init failed\r\n");
                if (mcrypt_generic_init (mdecryptfd, keybuf, sizeof(keybuf), IV) < 0)
                        errexit("\rmcrypt_generic_init failed\r\n");
        }


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
                errexit("socket() failed\n");

        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = INADDR_ANY; 
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		errexit("bind() failed\n");
	
	if (listen(sockfd, 5) == -1)
		errexit("listen() failed\n");
	
	connected_sockfd = accept(sockfd, NULL, NULL);
	if (connected_sockfd == -1)
		errexit("error on accept()\n");	



int ret = 0;
pipe1 = pipe(pipefd1);
pipe2 = pipe(pipefd2);
if (pipe1 == -1 || pipe2 == -1)
	errexit("Couldn't create pipe/s\n");

pid = fork();
if (pid != 0)  //parent process
{
	close(pipefd1[0]);
	close(pipefd2[1]);
	pollfds[0].fd = connected_sockfd;
	pollfds[1].fd = pipefd2[0];	
	pollfds[0].events = POLLIN | POLLHUP | POLLERR;
	pollfds[1].events = POLLIN | POLLHUP | POLLERR;	
while(1)
{
	ret = poll(pollfds, 2, 0);	
	if (ret == -1)
		errexit("Failed to poll\n");
	
	if (pollfds[0].revents & POLLIN)  //input from socket received
	{
		bytes_read = read(connected_sockfd, buf, 256);
		if (bytes_read == -1)
			errexit("Couldn't read from socket\n");
		int i;
		for (i = 0; i < bytes_read; i++)
		{
			if (encflag)  //decrypt before processing
			{
				if (mdecrypt_generic( mdecryptfd,&buf[i], 1) == -1)
					errexit("Couldn't decrypt message\n"); 
			}
			
printf("%c", buf[i]);

			if (buf[i] == 0x0A || buf[i] == 0x0D)
			{
				char lf = 0x0A;
				write(pipefd1[1], &lf, 1);
			}
			else if (buf[i] == 0x04)
			{
				close (pipefd1[1]);
				break;
			}
			else if (buf[i] == 0x03)
				kill(pid, SIGINT); 
			else
			{
			bytes_written = write(pipefd1[1], &buf[i], 1);
			if ((bytes_written == -1))
				errexit("Couldn't write to stdout/shell\n");
			}
		}
	}

	if (pollfds[1].revents & POLLIN)  //input from shell received
	{
		bytes_read = read(pipefd2[0], buf, 256);
		if (bytes_read == -1)
			errexit("Couldn't read from pipe\n");
		int i;
		for (i = 0; i < bytes_read; i++)
		{
			if (buf[i] == 0x04) //***is this end of file?
			{
				waitpid(0, &stats, WUNTRACED);
				SIG = stats & 0x007f;
                                STAT = stats & 0xff00;
                                STAT = STAT >> 8;
                                fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", SIG, STAT);
                           	close(connected_sockfd);
				if (encflag)
				{
					closeenc(mcryptfd);
					closeenc(mdecryptfd);
				}	
				printf("close connected socket\n");
				exit(0);
			}
			if (buf[i] == 0x0A)
			{
				char cr = 0x0D;
				if (encflag)
                                {
                                        if (mcrypt_generic (mcryptfd, &cr, 1) == -1)
                                                errexit("\rencryption failed\r\n");
                                }
				if (write(connected_sockfd, &cr, 1) == -1)
					errexit("1Couldn't write to socket\n");
			}
			if (encflag)  //encrypt before writing to socket
			{
				if (mcrypt_generic (mcryptfd, &buf[i], 1) == -1)
					errexit("\rencryption failed\r\n");
			}
			bytes_written = write(connected_sockfd, &buf[i], 1);
			if (bytes_written == -1)
				errexit("2Couldn't write to socket\n");
		}			
	}

	if (pollfds[1].revents & (POLLHUP | POLLERR))  //shell pipe EOF
	{
		waitpid(0, &stats, WUNTRACED);
		SIG = stats & 0x007f;
		STAT = stats & 0xff00;
		STAT = STAT >> 8;
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", SIG, STAT);
		if (encflag)
		{
			closeenc(mcryptfd);
			closeenc(mdecryptfd);
		}
		close(connected_sockfd);
		exit(0);	
	}

	if (pollfds[0].revents & (POLLHUP | POLLERR))  //connection EOF
	{
		close(pipefd1[1]);
/*
		waitpid(0, &stats, WUNTRACED);
		SIG = stats & 0x007f;
		STAT = stats & 0xff00;
		STAT = STAT >> 8;
		fprintf(stderr, "shell exit signal=%d status=%d\r\n", SIG, STAT);
		if (encflag)
		{
			closeenc(mcryptfd);
			closeenc(mdecryptfd);
		}
	exit(0);
*/
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
	
return 0;	
}

void errexit(char* errmsg)
{
	fprintf(stderr, errmsg);
	exit(1);
}

void closeenc(MCRYPT td)
{
        mcrypt_generic_deinit(td);
        mcrypt_module_close(td);
}

