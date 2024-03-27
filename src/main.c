#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
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

// Items weight
float *items_weight_vec;
size_t vec_capacity = 1500;
size_t vec_size = 0;
// float items_weight_vec[1500];

// Lock production for calculating total weight
int lock_production = 0;

void signal_handler(int signum) {
   printf("\n\n[+] Cleaning up...\n");
   free(items_weight_vec);
   unlink(SOCK_PATH);
   printf("[+] Exiting.\n");
   exit(signum);
}

int main() {
   // Set up signal handling for graceful termination
   signal(SIGINT, signal_handler);
   signal(SIGTERM, signal_handler);

   // Initial allocation for the items_weight_vec
   items_weight_vec = malloc(vec_capacity * sizeof(float));
   // Handle memory allocation failure
   if (items_weight_vec == NULL) {
      perror("[-] Failed to allocate items_weight_vec");
      exit(EXIT_FAILURE);
   }

   pid_t pid;
   pid = fork();

   if (pid < 0) // Error
   {
      perror("[-] Failed to fork child process for the display");
      exit(EXIT_FAILURE);
	}
   else if (pid == 0) // Child Process (Display)
   {
      // Reset signal handlers to default for the child process
      // so that only the parent process handles the custom signals
      signal(SIGINT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);

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
