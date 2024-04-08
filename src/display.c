#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "globals.h"

const int TWO_SECONDS_IN_MICROSECONDS = 2000000;

void clear_screen()
{
   printf("\033[2J");
   printf("\033[H");
}

int main()
{
   int server, length;
   struct sockaddr_un remote;
   char buffer[1024];

   // Create socket
   server = socket(AF_UNIX, SOCK_STREAM, 0);
   if (server < 0)
   {
      perror("[-] Client: Failed to create the socket");
      exit(EXIT_FAILURE);
   }

   // Connect to server
   memset(&remote, 0, sizeof(remote));
   remote.sun_family = AF_UNIX;
   strncpy(remote.sun_path, SOCK_PATH, sizeof(remote.sun_path) - 1);
   length = strlen(remote.sun_path) + sizeof(remote.sun_family);
   if (connect(server, (struct sockaddr *) &remote, length) < 0)
   {
      perror("[-] Client: Failed when connecting to server");
      close(server);
      exit(EXIT_FAILURE);
   }

   // Read data from server
   while (1)
   {
      if (read(server, buffer, sizeof(buffer)) < 0)
      {
         perror("[-] Client: Failed to read from socket");
         close(server);
         exit(EXIT_FAILURE);
      }

      clear_screen();

      // Print data
      printf("\n========== Display ==========\n");
      printf("%s\n", buffer);
      printf("=============================\n\n");

      // It should read data every 2 seconds
      usleep(TWO_SECONDS_IN_MICROSECONDS);
   }

   // Close socket and exit
   close(server);
   exit(EXIT_SUCCESS);
}
