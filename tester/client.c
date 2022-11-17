#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <dirent.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFSIZE 2048

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in dstaddr;
	struct hostent *dstent;
	char buf[BUFSIZE];
	ssize_t nread;
	char *bufp;
	size_t bufsz;

	if (argc != 3) {
		printf("Invalid number of arguments.\n");
		return 1;
	}

	printf("Creating socket...\n");
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		printf("Unable to create socket.\n");
		goto esock;
	}
	printf("Done.\n");

	dstaddr.sin_family = AF_INET;
	dstaddr.sin_port = htons(atoi(argv[2]));
	/*
	dstent = gethostbyname(argv[1]);
	memcpy(&dstaddr.sin_addr.s_addr, dstent->h_addr, dstent->h_length);
	*/
	dstaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	printf("Connecting to destination socket...\n");
	if (connect(sock, (struct sockaddr *)&dstaddr, sizeof(dstaddr)) < 0) {
		printf("Unable to connect\n");
		goto econnect;
	}
	printf("Done.\n");

	bufp = buf;
	bufsz = BUFSIZE;
	while ((nread = getline(&bufp, &bufsz, stdin)) > 0) {
		buf[nread-1] = '\r';
		buf[nread] = '\n';
		buf[nread+1] = '\0';
		send(sock, buf, nread+1, 0);
	}

	while ((nread = read(sock, buf, BUFSIZE)) > 0)
		write(1, buf, nread);

econnect:
	close(sock);
esock:
	perror(0);
	return 0;
}
