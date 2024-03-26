#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "globals.h"
#include "belt.h"
#include "display.h"

// Socket path for the pipe connection
const char *SOCK_PATH = "/tmp/foodsec_pipe";

// Total items count
int total_items_count = 0;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

// Total items weight
float total_items_weight = 0.0;
pthread_mutex_t weight_mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
   // Fork a child process
   pid_t pid;
   pid = fork();

   if (pid < 0) // Error
   {
      perror("[-] Failed to fork child process for the display");
      exit(EXIT_FAILURE);
	}
   else if (pid == 0) // Child Process (Display)
   {
      printf("[+] Initializing display client...\n");
      init_display_client();
   }
   else // Parent Process (Conveyor Belt threads)
   {
      printf("[+] Initializing belt server...\n");
      init_belt_server();
   }

   return EXIT_SUCCESS;
}
