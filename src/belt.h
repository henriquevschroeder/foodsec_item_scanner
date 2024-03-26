#ifndef BELT_HEADER
#define BELT_HEADER

typedef struct {
   int id;
   float item_weight;
   float speed;
   int wait_time_in_microsseconds;
} BeltData;

void *belt_thread(void *arg);

#endif
