#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "globals.h"
#include "belt.h"

const int BELT_NUM = 3;

void *belt_thread(void *arg)
{
   BeltData *belt_data = (BeltData *)arg;
   
   printf(
      "[+] Server: Initializing Belt %d | Weight='%.2f Kg' | Interval='%.2f' s\n",
      belt_data->id, belt_data->item_weight, belt_data->wait_time_in_microsseconds / 1000000.0
   );

   while (1)
   {
      usleep(belt_data->wait_time_in_microsseconds);

      pthread_mutex_lock(&count_mutex);
      total_items_count += 1;
      pthread_mutex_unlock(&count_mutex);

      pthread_mutex_lock(&weight_mutex);
      total_items_weight += belt_data->item_weight;
      pthread_mutex_unlock(&weight_mutex);

      char buffer[1024];
      sprintf(buffer, "Count: %d\nWeight: %.2f", total_items_count, total_items_weight);

      // Write processed data back to client
      if (write(belt_data->newsockfd, buffer, strlen(buffer) + 1) < 0)
      {
         perror("[-] Failed to write in socket");
         close(belt_data->newsockfd);
         close(belt_data->sockfd);
         pthread_exit(NULL);
      }

      printf("\nBelt %d: added 1 item with %.2f kg", belt_data->id, belt_data->item_weight);
   }
}

void init_belt_server()
{
   int sockfd, newsockfd, len;
   struct sockaddr_un local, remote;

   // Create socket
   sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
   if (sockfd < 0)
   {
      perror("[-] Server: Failed to create the socket");
      exit(EXIT_FAILURE);
   }

   // Bind socket to local address
   memset(&local, 0, sizeof(local));
   local.sun_family = AF_UNIX;
   strncpy(local.sun_path, SOCK_PATH, sizeof(local.sun_path) - 1);
   unlink(local.sun_path);
   len = strlen(local.sun_path) + sizeof(local.sun_family);
   if (bind(sockfd, (struct sockaddr *)&local, len) < 0)
   {
      perror("[-] Server: Failed to capture the socket");
      close(sockfd);
      exit(EXIT_FAILURE);
   }

   printf("[+] Server: Listening on %s...\n", SOCK_PATH);

   // Listen for connections
   if (listen(sockfd, 5) < 0)
   {
      perror("[-] Server: Failed to listen the socket");
      close(sockfd);
      exit(EXIT_FAILURE);
   }

   // Accept connections
   memset(&remote, 0, sizeof(remote));
   len = sizeof(remote);
   newsockfd = accept(sockfd, (struct sockaddr *)&remote, &len);
   if (newsockfd < 0)
   {
      perror("[-] Server: Failed to accept connection");
      close(sockfd);
       exit(EXIT_FAILURE);
   }

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
         exit(EXIT_FAILURE);
      }

      // Initialize the belt data
      belt_data[i]->id = i;
      belt_data[i]->newsockfd = newsockfd;
      belt_data[i]->sockfd = sockfd;
      switch (i)
      {
         case 0:
            belt_data[i]->item_weight = 5.0; // weight in kg
            belt_data[i]->wait_time_in_microsseconds = 1000000; // 1 second
            break;
         case 1:
            belt_data[i]->item_weight = 2.0; // weight in kg
            belt_data[i]->wait_time_in_microsseconds = 500000; // 0.5 second
            break;
         case 2:
            belt_data[i]->item_weight = 0.5; // weight in kg
            belt_data[i]->wait_time_in_microsseconds = 100000; // 0.1 second
            break;
      } 

      if (pthread_create(&threads[i], &attr, belt_thread, (void *)belt_data[i]) != 0)
      {
         perror("[-] Failed on pthread_create");
         exit(EXIT_FAILURE);
      }
   }

   // Wait for the threads to finish
   for (int i = 0; i < BELT_NUM; i++)
   {
      pthread_join(threads[i], NULL);
      free(belt_data[i]);
   }
}
