#include <pthread.h>
#include <stdio.h>

typedef struct {

    char * filename;

} t_file;

typedef struct {

    char id[8];
    char description[40];
    char min_stock[5];

} t_maestro;

typedef struct {

    char id[8];
    char stock[5];

} t_stock;

typedef struct {

    FILE * log;
    int cant_articulos;
    pthread_t threadId;
    int       threadNum;

} t_thread;

typedef struct t_node {

    struct t_node *next;
    t_file data;

} t_node;

typedef struct {

    t_node *first;
    t_node *last;

} t_queue;