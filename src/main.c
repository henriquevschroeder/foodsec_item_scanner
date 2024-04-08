#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include "globals.h"
#include "main.h"

// Total conveyor belts
const int BELT_NUM = 3;

// Items count
int total_items_count = 0;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

// Items weight
double total_items_weight = 0;
double items_weight_vec[1500];
int items_weight_vec_size = 0;

// Control lock state for calculating total weight
int is_counting_stopped = 0;
int is_weight_summed = 0;

// Socket Pipe configuration
struct sockaddr_un local, remote;
int server, client, length;

void sum_total_weight()
{
   double sum = 0;
   for (int i = 0; i < items_weight_vec_size; i++)
   {
      sum += items_weight_vec[i];
   }
   total_items_weight += sum;
}

void add_weight(double weight)
{
   if (items_weight_vec_size % 1500 == 0)
   {
      items_weight_vec_size = 0;
   }

   items_weight_vec[items_weight_vec_size] = weight;
   items_weight_vec_size++;
   total_items_count++;
}

void create_pipe_connection()
{
   // Create socket
   server = socket(AF_UNIX, SOCK_STREAM, 0);
   if (server < 0)
   {
      perror("[-] Server: Failed to create the socket");
      exit(EXIT_FAILURE);
   }

   // Bind socket to local address
   memset(&local, 0, sizeof(local));
   local.sun_family = AF_UNIX;
   strncpy(local.sun_path, SOCK_PATH, sizeof(local.sun_path) - 1);
   unlink(local.sun_path);
   length = strlen(local.sun_path) + sizeof(local.sun_family);
   if (bind(server, (struct sockaddr *) &local, length) < 0)
   {
      perror("[-] Server: Failed to capture the socket");
      close(server);
      exit(EXIT_FAILURE);
   }

   printf("[+] Server: Listening on %s...\n", SOCK_PATH);

   // Listen for connections
   if (listen(server, 5) < 0)
   {
      perror("[-] Server: Failed to listen the socket");
      close(server);
      exit(EXIT_FAILURE);
   }

   // Accept connections
   memset(&remote, 0, sizeof(remote));
   length = sizeof(remote);
   client = accept(server, (struct sockaddr *) &remote, &length);
   if (client < 0)
   {
      perror("[-] Server: Failed to accept connection");
      close(server);
      close(client);
      exit(EXIT_FAILURE);
   }

   printf("[+] Server: Connection accepted\n");
}

void *belt_thread(void *arg)
{
   BeltData *belt_data = (BeltData *) arg;

   while (1)
   {
      // Wait for the belt to move
      usleep(belt_data->wait_time_in_microsseconds);

      pthread_mutex_lock(&count_mutex);

      if (!is_weight_summed && total_items_count != 0 && total_items_count % 1500 == 0)
      {
         is_weight_summed = 1;
         sum_total_weight();
      }
      else
      {
         is_weight_summed = 0;
         add_weight(belt_data->item_weight);
      }

      pthread_mutex_unlock(&count_mutex);

      char buffer[1024];
      sprintf(buffer, "Count: %d\nTotal Weight: %.2f", total_items_count, total_items_weight);

      // Write processed data back to client
      if (write(belt_data->client, buffer, strlen(buffer) + 1) < 0)
      {
         perror("[-] Failed to write in socket");
         close(belt_data->client);
         close(belt_data->server);
         pthread_exit(NULL);
      }
   }
}

void stop_belts() {
   is_counting_stopped = !is_counting_stopped;

   if (is_counting_stopped)
   {
      printf("[+] Server: All belts stopping...\n");
      pthread_mutex_lock(&count_mutex);
      printf("[+] Server: All belts stopped\n");
   }
   else
   {
      printf("[+] Server: All belts resuming...\n");
      pthread_mutex_unlock(&count_mutex);
      printf("[+] Server: All belts resumed\n");
   }
}

void clear_and_exit()
{
   close(client);
   close(server);
   exit(EXIT_SUCCESS);
}

int main()
{
   // User can press CTRL+C to stop/resume the conveyor belts
   signal(SIGINT, stop_belts);

   pthread_mutex_init(&count_mutex, NULL);

   create_pipe_connection();

   BeltData *belt_data[BELT_NUM];
   pthread_t threads[BELT_NUM];
   pthread_attr_t attr;
   pthread_attr_init(&attr);

   // Create the BELT threads
   for (int i = 0; i < BELT_NUM; i++)
   {
      belt_data[i] = malloc(sizeof(BeltData));
      if (belt_data[i] == NULL)
      {
         perror("[-] Failed to allocate memory for belt data");
         clear_and_exit();
      }

      // Initialize belt data
      belt_data[i]->id = i;
      belt_data[i]->client = client;
      belt_data[i]->server = server;
      switch (i)
      {
         case 0:
            belt_data[i]->item_weight = 5; // 5 kg
            belt_data[i]->wait_time_in_microsseconds = 1000000; // 1 second
            break;
         case 1:
            belt_data[i]->item_weight = 2; // 2 kg
            belt_data[i]->wait_time_in_microsseconds = 500000; // 0.5 second
            break;
         case 2:
            belt_data[i]->item_weight = 0.5; // 0.5 kg
            belt_data[i]->wait_time_in_microsseconds = 100000; // 0.1 second
            break;
      }

      if (pthread_create(&threads[i], &attr, belt_thread, (void *) belt_data[i]) != 0)
      {
         perror("[-] Failed on pthread_create");
         clear_and_exit();
      }
   }

   // Wait for the threads to finish
   for (int i = 0; i < BELT_NUM; i++)
   {
      pthread_join(threads[i], NULL);
      free(belt_data[i]);
   }

   pthread_mutex_destroy(&count_mutex);
   close(client);
   close(server);

   return EXIT_SUCCESS;
}
