#ifndef BELT_H
#define BELT_H

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
