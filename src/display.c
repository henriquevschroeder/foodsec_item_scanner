#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include "globals.h"

const int TWO_SECONDS_IN_MICROSECONDS = 2000000;

void init_display_client()
{
	int sockfd, len;
	struct sockaddr_un remote;
	char buffer[1024];

	sleep(1); // Wait for server to start

	// Create socket
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("[-] Client: Failed to create the socket");
		exit(EXIT_FAILURE);
	}

	// Connect to server
	memset(&remote, 0, sizeof(remote));
	remote.sun_family = AF_UNIX;
	strncpy(remote.sun_path, SOCK_PATH, sizeof(remote.sun_path) - 1);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sockfd, (struct sockaddr *)&remote, len) < 0)
	{
		perror("[-] Client: Failed when connecting to server");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// Read data from server
	while(1)
	{
		usleep(TWO_SECONDS_IN_MICROSECONDS);

		if (read(sockfd, buffer, sizeof(buffer)) < 0)
		{
			perror("[-] Client: Failed to read from socket");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
		
		printf("\n========== Display ==========\n");
		printf("%s\n", buffer);
		printf("=============================\n\n");
	}
}
