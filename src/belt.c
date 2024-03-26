#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "globals.h"
#include "belt.h"

void *belt_thread(void *arg)
{
   BeltData *belt_data = (BeltData *)arg;
   printf(
      "Belt %d | Item Weight = %.2f Kg | Speed = %.2f items/second | Wait Time %i\n",
      belt_data->id, belt_data->item_weight, belt_data->speed, belt_data->wait_time_in_microsseconds
   );

   while (1)
   {
      usleep(belt_data->wait_time_in_microsseconds);
      pthread_mutex_lock(&count_mutex);
      total_items_count += 1;
      pthread_mutex_unlock(&count_mutex);
      printf("Belt %d: added 1 item\n", belt_data->id);
   }
}
