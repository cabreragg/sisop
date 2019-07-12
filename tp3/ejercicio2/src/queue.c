#include "queue.h"

void createQueue(t_queue *queue){

    queue->first=NULL;
    queue->last=NULL;
}

int enqueue(t_queue *queue, const t_file *data) {

    t_node *aux=(t_node*)malloc(sizeof(t_node));

    if( aux == NULL)
        return OUT_OF_MEMORY;

    aux->data=*data;
    aux->next=NULL;

    if (queue->first == NULL)
        queue->first = aux;
    else
        queue->last->next = aux;

    queue->last = aux;

    return OK;
}

int dequeue(t_queue *queue, t_file *data) {

    t_node *aux;

    if (queue->first == NULL)
        return EMPTY;

    *data = queue->first->data;
    aux = queue->first;
    queue->first = aux->next;
    
    if (queue->first == queue->last)
        queue->last = NULL;

    free(aux);

    return OK;
}
void emptyQueue(t_queue *queue) {

    t_node *aux;
    
    while (queue->first) {
        if (queue->first == queue->last)
            queue->last = NULL;

        aux = queue->first;
        queue->first = aux->next;
        
        free(aux);
    }
}

int isQueueEmpty(const t_queue *queue) {

    if(queue->first == NULL)
        return EMPTY;

    return OK;
}

void showQueue(const t_queue *queue){

    t_file *queueNode=(t_file*)malloc(sizeof(t_file));
    t_node *aux;

    aux = queue->first;
    *queueNode = aux->data;

    printf("%s\n", queueNode->filename);

    while (aux->next){
        aux = aux->next;
        
        *queueNode=aux->data;

        printf("%s\n", queueNode->filename);
    }
}
