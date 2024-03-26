#ifndef BELT_HEADER
#define BELT_HEADER

typedef struct {
   int id;
   float item_weight;
   int wait_time_in_microsseconds;
   int newsockfd;
   int sockfd;
} BeltData;

void *belt_thread(void *arg);
void init_belt_server();

#endif
