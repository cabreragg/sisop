#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

#define OUT_OF_MEMORY -1
#define EMPTY 1
#define OK 0

void createQueue(t_queue *queue);
int enqueue(t_queue *queue,const t_file *data);
int dequeue(t_queue *queue,t_file *data);
void emptyQueue(t_queue *queue);
int isQueueEmpty(const t_queue *queue);
void showQueue(const t_queue *queue);
