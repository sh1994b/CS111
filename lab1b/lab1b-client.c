
#include <mcrypt.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>

void closeenc(MCRYPT sockfd);
void restore(struct termios attrscp, int exitcode);
void errexit(char* error, int exitcode);
struct termios attrsCopy;


int main(int argc, char* argv[])
{
	int fd0 = 0, opt = -1, attr = 0, sockfd = 0, keyfd = 0, logfd = 0;
	int port = -1;
	char* keyfile = "my.key";
	char* logfile = "log.txt";
	char* hostname = "localhost";
	struct termios terminalAttrs;
	ssize_t bytes_read = 0, bytes_written = 0;
	char buf[256];
	char kbuf[256];
	int long_index = 0;
	int encflag = 0, logflag = 0; 
	char keybuf[10];
	char *IV;
	
	MCRYPT mcryptfd, mdecryptfd;
	if (argc < 2)
	{
		fprintf(stderr, "Please enter port number as an argument\n");
		exit(1);
	}
	attr = tcgetattr(0, &terminalAttrs);
	if (attr == -1)
	{
		fprintf(stderr, "Couldn't save current terminal modes\n");
		exit(1);
	}

	attrsCopy = terminalAttrs;

	terminalAttrs.c_iflag = ISTRIP;	
	terminalAttrs.c_oflag = 0;	
	terminalAttrs.c_lflag = 0;	


	static struct option long_options[] =
	{
		{"port=", required_argument, NULL, 'p'},
		{"encrypt=", required_argument, NULL, 'e'},
		{"log=", required_argument, NULL, 'l'},
		{"host=", required_argument, NULL, 'h'},
		{0,0,0,0}
	};
char *temp;
	while ((opt = getopt_long(argc, argv, "p:e:l:h:", long_options, &long_index)) != -1)
	{
		switch (opt)
		{
			case 'p':
				temp = optarg;
				port = atoi(temp);
				break;
			case 'e':
				keyfile = optarg;
				encflag = 1;
				break;
			case 'l':
				logfile = optarg;
				logflag = 1;
				break;
			case 'h':
				hostname = optarg;
				break;
			default:
				errexit("\rUnrecognized option entered\r\n",1 );
		}
	}


	

	if (encflag)
	{
		int i;
		mcryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
		mdecryptfd = mcrypt_module_open("twofish", NULL, "cfb", NULL);
		if (mcryptfd == MCRYPT_FAILED)
			errexit("\rmcrypt_module_open failed for mcrypt\r\n", 1);
		if (mdecryptfd == MCRYPT_FAILED)
			errexit("\rmcrypt_module_open failed for mdecrypt\r\n", 1);
		keyfd = open(keyfile, O_RDONLY, 0444);
		if (keyfd == -1)
			errexit("\rCouldn't open key file\r\n", 1);
		bytes_read = read (keyfd, keybuf, sizeof(keybuf));
		IV = malloc(mcrypt_enc_get_iv_size(mcryptfd));
		for (i=0; i< mcrypt_enc_get_iv_size(mcryptfd); i++)
			IV[i]=i;
		if (mcrypt_generic_init (mcryptfd, keybuf, sizeof(keybuf), IV) < 0)
			errexit("\rmcrypt_generic_init failed\r\n", 1);
		if (mcrypt_generic_init (mdecryptfd, keybuf, sizeof(keybuf), IV) < 0)
			errexit("\rmcrypt_generic_init failed\r\n", 1);
		close(keyfd);
	}

	if (logflag)
	{
		logfd = open(logfile, O_CREAT | O_WRONLY, 0644);
		if (logfd == -1)
			errexit("\rCouldn't open log file\r\n", 1);
		if(access(logfile, W_OK) == -1)
			errexit("\rlog file not writable\r\n", 1);
	}

	if (tcsetattr(0, TCSANOW, &terminalAttrs) < 0)
	{
		fprintf(stderr, "tcsetattr failed\n");
		exit(1);
	}

	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		errexit("\rsocket() failed\r\n", 1);
	struct sockaddr_in serv_addr;
	struct hostent *server = gethostbyname(hostname);
	if (server == NULL)
		errexit("\rhost does not exist\r\n", 1);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		errexit("\rconnect() failed\r\n", 1);

	struct pollfd pollfds[2];
	pollfds[0].fd = 0;
	pollfds[1].fd = sockfd;
	pollfds[0].events = POLLIN | POLLHUP | POLLERR;
	pollfds[1].events = POLLIN | POLLHUP | POLLERR;

	int ret = 0;


	while (1)
	{
		ret = poll(pollfds, 2, -1);
		if (ret == -1)
			errexit("\rpoll() failed\r\n", 1);
		
		if (pollfds[0].revents & POLLIN)  //input from keyboard received
		{
			bytes_read = read (fd0, kbuf, 256);
			if (bytes_read == -1)
				errexit("\rCouldn't read from stdin\r\n", 1);
			int i;
			for (i = 0; i < bytes_read; i++)
			{
				if (kbuf[i] == 0x0A || kbuf[i] == 0x0D)
                		{
                        		char cr = 0x0D;
                        		char lf = 0x0A;
                        		write(1, &cr, 1);
                        		write(1, &lf, 1);
                		}
				else
					write(1, &kbuf[i], 1);
				//encrypt before sending to server
				if (encflag)
				{
					if (mcrypt_generic (mcryptfd, &kbuf[i], 1) == -1)
						errexit("\rencryption failed\r\n", 1);
				}
				bytes_written = write(sockfd, &kbuf[i], 1);
				if (bytes_written == -1)
					errexit("\rCouldn't write to stdout\r\n", 1);
			}
			if (logflag)
			{
			int temp = 0;
			temp = dprintf(logfd, "SENT %d bytes: %.*s\n", (int)bytes_read, (int)bytes_read, kbuf);
			if (temp == -1)
				errexit("Couldn't write to log file\r\n", 1);
			}
		}

		if (pollfds[1].revents & POLLIN)  //input from socket received
		{	
			bytes_read = read (sockfd, buf, 256);
			if (bytes_read == -1)
				errexit("\rCouln't read from socket\r\n", 1);	
			if (bytes_read == 0)
				restore(attrsCopy,0);
			int i;
			if (logflag)
				{
			int temp = 0;
			temp = dprintf(logfd, "RECEIVED %d bytes: %s\r\n", (int)bytes_read, buf);
			if (temp == -1)
				errexit("Couldn't write to log file\r\n", 1);
	
				}
			for (i = 0; i < bytes_read; i++)
                	{
				if (encflag)  //decrypt before echoing to screen
				{
					if(mdecrypt_generic(mdecryptfd, &buf[i], 1) == -1)
						errexit("\rdecryption failed\r\n", 1);
				}
                        	if (buf[i] == 0x04)
				{
                                	restore(attrsCopy, 0);;
					break;
				}
                        	if (buf[i] == 0x0A)
                        	{
                                	char cr = 0x0D;
                                	write(1, &cr, 1);
                        	}
                        	bytes_written = write(1, &buf[i], 1);
                        	if (bytes_written == -1)
                                	errexit("\rCouldn't write to stdout\r\n", 1);
                	}

		}

		if ((pollfds[1].revents & (POLLHUP | POLLERR)))
	        {
			if (encflag)
			{
				closeenc(mcryptfd);
				closeenc(mdecryptfd);
			}
			tcsetattr(0, TCSANOW, &attrsCopy);
			exit(0);
	
        	}


	}
	if (encflag)
		free(IV);
	if (logflag)
		close(logfd);
	restore(attrsCopy, 0);
	return 0;	
}

void restore(struct termios attrscp, int exitcode)
{
	tcsetattr(0, TCSANOW, &attrscp);
	exit(exitcode);
}

void errexit(char* error, int exitcode)
{
	fprintf(stderr, error);
	restore(attrsCopy, exitcode);
}

void closeenc(MCRYPT td)
{
	mcrypt_generic_deinit(td);
	mcrypt_module_close(td);
}
