#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "globals.h"

const int TWO_SECONDS_IN_MICROSECONDS = 2000000;

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;

void *display_count_thread(void)
{
	while(1)
	{
		usleep(TWO_SECONDS_IN_MICROSECONDS);
		pthread_mutex_lock(&screen_mutex);
		printf("\n============== Display ==============\n");
		printf("- Item Count: %i\n", total_items_count);
		printf("- Total Weight: %.2f kg\n", total_items_weight);
		printf("============== Display ==============\n\n");
		pthread_mutex_unlock(&screen_mutex);
	}
}
