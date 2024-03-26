#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "globals.h"
#include "belt.h"
#include "display.h"

const int BELT_NUM = 3;

// Total items count
int total_items_count = 0;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

// Total items weight
float total_items_weight = 0.0;
pthread_mutex_t weight_mutex = PTHREAD_MUTEX_INITIALIZER;;

int main()
{
   BeltData *belt_data[BELT_NUM];
   pthread_t threads[BELT_NUM], display_thread; 
   pthread_attr_t attr;
	pthread_attr_init(&attr);

   // Create the BELT threads
   for (int i = 0; i < BELT_NUM; i++)
   {
      belt_data[i] = malloc(sizeof(BeltData));
      if (belt_data[i] == NULL)
      {
         perror("Failed to allocate memory for belt data");
         exit(EXIT_FAILURE);
      }

      // Initialize the belt data
      belt_data[i]->id = i;
      switch (i)
      {
         case 0:
            belt_data[i]->item_weight = 5.0; // weight in kg
            belt_data[i]->speed = 1.0; // items per second
            belt_data[i]->wait_time_in_microsseconds = 1000000; // 1 second
            break;
         case 1:
            belt_data[i]->item_weight = 2.0; // weight in kg
            belt_data[i]->speed = 2.0; // items per second (1 item every 0.5 second)
            belt_data[i]->wait_time_in_microsseconds = 500000; // 0.5 second
            break;
         case 2:
            belt_data[i]->item_weight = 0.5; // weight in kg
            belt_data[i]->speed = 10.0; // items per second (1 item every 0.1 second)
            belt_data[i]->wait_time_in_microsseconds = 100000; // 0.1 second
            break;
      } 

      if (pthread_create(&threads[i], NULL, belt_thread, (void *)belt_data[i]) != 0)
      {
         perror("Failed on pthread_create");
         exit(EXIT_FAILURE);
      }
   }

   // Create the DISPLAY thread
   if (pthread_create(&display_thread, &attr, display_count_thread, NULL) != 0)
   {
      perror("Failed on pthread_create");
      exit(EXIT_FAILURE);
   }

   // Wait for the threads to finish
   for (int i = 0; i < BELT_NUM; i++)
   {
      pthread_join(threads[i], NULL);
      free(belt_data[i]);
   }
   pthread_join(display_thread, NULL);

   return EXIT_SUCCESS;
}
