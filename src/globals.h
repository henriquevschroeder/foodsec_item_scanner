#include <pthread.h>

#ifndef GLOBALS_H
#define GLOBALS_H

extern int total_items_count;
extern pthread_mutex_t count_mutex;

extern float total_items_weight;
extern pthread_mutex_t weight_mutex;

extern const char* SOCK_PATH;

#endif