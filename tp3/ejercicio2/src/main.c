/*
Ejercicio2: main.c
TP3 - Ejercicio 2

Integrantes:
    Bouza Facundo 	DNI 41332191
    Cabrera Gonzalo	DNI 37350600
    Facal Ernesto 	DNI 38983722
    Marson Tomás	DNI 40808276

Número de entrega: segunda reentrega.
*/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "queue.h"

#define MASTER_REG_SIZE 54
#define STOCK_REG_SIZE 14
#define MAX_THREAD_COUNT 1000
#define MASTER_FILE_NAME "articulos.txt"
#define FORMATO_SUCURSALES "Sucursal_"
#define READ_MODE "r"
#define WRITE_MODE "w"

int process(int , char *, long );
int enqueueStockFiles(char *);
char * substr(char *, int, int);
long calcularArticulos(char *);
void createOutputFiles(FILE **, FILE **, FILE **, char *);
void * processFiles(void *);
void loadMasterFile(char *);
void printHelp();

t_queue queue;
pthread_mutex_t m_queue, m_log, m_stock;
long *stock_arr, *min_stock_arr, *ids_arr;

int main(int argc, char *argv[]) {

	DIR *d;
	struct dirent *dir;
	char path[1024], path_sucursal[1024], path_master[1024], *p;
	long cant_threads, cant_articulos = 0;

	int c = getopt(argc, argv, "hH");

	switch(c) {
		case 'h':
		case 'H':
			printHelp();
			return 0;
		case -1:
			break;
		default:
			printf("Error en la llamada. Intente './ejercicio2 -help' para mas informacion.\n");
			return 1;
	}

	if (argc != 3) {
		printf("Error en la llamada. Intente './ejercicio2 -help' para mas informacion.\n");
		return 1;
	}

	cant_threads = strtol(argv[1], &p, 10);

	if (!(*p == '\n' || *p == '\0')) {
		printf("La cantidad de threads ingresada '%s' es invalida.\n", argv[1]);
		return 1;
	}

  	d = opendir(argv[2]);

	if (d == NULL) {
		printf("El directorio ingresado '%s' es invalido.\n", argv[2]);
		return 1;
	}

	realpath(argv[2], path);
	snprintf(path_master, sizeof(path_master), "%s/%s", path, MASTER_FILE_NAME);

	cant_articulos = calcularArticulos(path_master);

	ids_arr = (long *)malloc(sizeof(long) * cant_articulos);
	min_stock_arr = (long *)malloc(sizeof(long) * cant_articulos);
	stock_arr = (long *)malloc(sizeof(long) * cant_articulos);

	loadMasterFile(path_master);

	while ((dir = readdir(d)) != NULL) {

		if(strstr(dir->d_name, FORMATO_SUCURSALES) != NULL) {

			snprintf(path_sucursal, sizeof(path_sucursal), "%s/", path);
			strcat(path_sucursal, dir->d_name);

			if(enqueueStockFiles(path_sucursal)) {
				printf("No se pudo procesar el archivo de stock '%s'.\n", path_sucursal);
			}
		}
	}

	closedir(d);

  	process(cant_threads, argv[0], cant_articulos);

	return 1;
}

int process(int cant_threads, char *path, long cant_articulos) {

	FILE *purchase_ptr, *stock_ptr, *log_ptr;
	t_thread *threads;

	char id[9], stock[10], compra[10];

	createOutputFiles(&purchase_ptr, &stock_ptr, &log_ptr, path);

	if (isQueueEmpty(&queue)) {
		printf("No se encontro ningun archivo para procesar.\n");
		fprintf(log_ptr, "No se encontro ningun archivo para procesar.\n");
		exit(1);
	}

	threads = (t_thread *)calloc(cant_threads, sizeof(t_thread));
	
	if (threads == NULL) {
		printf("No se pudieron crear los threads. Cerrando proceso...\n");
		fprintf(log_ptr, "No se pudieron crear los threads. Cerrando proceso...\n");
		return 1;
	}

	pthread_mutex_init(&m_log, NULL);
    pthread_mutex_init(&m_stock, NULL);
    pthread_mutex_init(&m_queue, NULL);

	for (int i = 0; i < cant_threads; i++) {

		threads[i].threadNum = i + 1;
		threads[i].cant_articulos = cant_articulos;
		threads[i].log = log_ptr;

		int rc = pthread_create(&threads[i].threadId, NULL, processFiles, &threads[i]);
        if (rc) {
            printf("Error '%d' al crear thread ID: '%d'. Cerrando proceso...\n", rc, threads[i].threadNum);
			fprintf(log_ptr, "Error '%d' al crear thread ID: '%d'. Cerrando proceso...\n", rc, threads[i].threadNum);
			exit(1);
        }
	}

	for (int i = 0; i < cant_threads; i++) {

		int rc = pthread_join(threads[i].threadId, NULL);
		if (rc) {
            printf("Error '%d' al hacer el join del thread ID: '%d'. Cerrando proceso...\n", rc, threads[i].threadNum);
			fprintf(log_ptr, "Error '%d' al hacer el join del thread ID: '%d'. Cerrando proceso...\n", rc, threads[i].threadNum);
			exit(1);
        }
	}

	int pos = 0;

	while (pos < cant_articulos) {

		snprintf(id, sizeof(id), "%08ld", ids_arr[pos]);

		snprintf(stock, sizeof(stock), "%07ld", stock_arr[pos]);


		if (min_stock_arr[pos] > stock_arr[pos]) {
			snprintf(compra, sizeof(compra), "%07ld", (min_stock_arr[pos] * 2) - stock_arr[pos]);
			fprintf(purchase_ptr, "%s %s\n", id, compra);
		}

		fprintf(stock_ptr, "%s %s\n", id, stock);

		pos++;
	}

	fprintf(log_ptr, "Se procesaron todos los archivos. Saliendo...\n");
	printf("Se procesaron todos los archivos. Saliendo...\n");

	fclose(log_ptr);
	fclose(purchase_ptr);
	fclose(stock_ptr);

	return 1;
}

void * processFiles(void *thread_info) {

	t_thread *thread = (t_thread*)thread_info;
	t_file *data = (t_file *)malloc(sizeof(t_file));
	FILE *sucursal_ptr;

	char *id, *stock, line[256];

	while (!isQueueEmpty(&queue)) {

		pthread_mutex_lock(&m_queue);

		if(isQueueEmpty(&queue)){
			pthread_mutex_unlock(&m_queue);
			break;
		}

		if (dequeue(&queue, data) == OK) {
			pthread_mutex_unlock(&m_queue);
			
			sucursal_ptr = fopen(data->filename, READ_MODE);
		} else {
			pthread_mutex_unlock(&m_queue);
			pthread_mutex_lock(&m_log);

			fprintf(thread->log, "Thread '%02d' no pudo desencolar archivo. Continuando...\n", thread->threadNum);

			pthread_mutex_unlock(&m_log);

			continue;
		}

		if (sucursal_ptr == NULL) {
			pthread_mutex_lock(&m_log);

			fprintf(thread->log, "Thread '%02d' no pudo abrir el archivo de sucursal '%s'. Continuando...\n", thread->threadNum, data->filename);

			pthread_mutex_unlock(&m_log);
			continue;
		}

		pthread_mutex_lock(&m_log);
			
		fprintf(thread->log, "Thread '%02d' procesa el archivo de la sucursal '%s'.\n", thread->threadNum, data->filename);
		printf("Thread '%02d' procesa el archivo de la sucursal '%s'.\n", thread->threadNum, data->filename );
			
		pthread_mutex_unlock(&m_log);

		while (fgets(line, sizeof(line), sucursal_ptr) != NULL) {
				
			id = substr(line, 1, 8);
			stock = substr(line, 9, 5);

			int pos = 0, found = 0;
			long id_l;
			char *p;

			while (pos < thread->cant_articulos && found == 0) {
				
				id_l = strtol(id, &p, 10);

				if (id_l == ids_arr[pos]) {
					found = 1;
					break;
				}
					
				pos++;
			}

			if (found) {
				pthread_mutex_lock(&m_stock);
				
				stock_arr[pos] += strtol(stock, &p, 10);
				
				pthread_mutex_unlock(&m_stock);
			} else {
				pthread_mutex_lock(&m_log);
				
				fprintf(thread->log, "Thread '%d' - No se encontro el articulo '%8s' en el maestro de articulos.\n", thread->threadNum, id);
				
				pthread_mutex_unlock(&m_log);
			}
		}

		fclose(sucursal_ptr);
	}
}

void createOutputFiles(FILE **purchase_order, FILE **stock, FILE **log, char *path) {
  
	char output[FILENAME_MAX], consolidated[FILENAME_MAX], purchase[FILENAME_MAX], log_file[FILENAME_MAX];

	snprintf(output, sizeof(output), "%s%s", dirname(path), "/output");
	mkdir(output, 0700);

	snprintf(purchase, sizeof(purchase), "%s%s", output, "/pedido_compras");
  	*purchase_order = fopen(purchase, WRITE_MODE);

	if (*purchase_order == NULL) {
		printf("No se pudo crear el archivo '%s'.\n", purchase);
		exit(1);
	}

	snprintf(consolidated, sizeof(consolidated), "%s%s", output, "/stock_consolidado");
  	*stock = fopen(consolidated, WRITE_MODE);

	if (*stock == NULL) {
		printf("No se pudo crear el archivo '%s'.\n", consolidated);
		exit(1);
	}

	snprintf(log_file, sizeof(log_file), "%s%s", output, "/log");
  	*log = fopen(log_file, WRITE_MODE);

	if (*log == NULL) {
		printf("No se pudo crear el archivo '%s'.\n", log_file);
		exit(1);
	}

  	return;
}

int enqueueStockFiles(char *fname) {

	t_file *aux = (t_file *)malloc(sizeof(t_file));
	aux->filename = malloc(FILENAME_MAX);

	strcpy(aux->filename, fname);

	if (isQueueEmpty(&queue)) {
		createQueue(&queue);
	}

	if (enqueue(&queue, aux)) {
		return 1;
	}

	return 0;
}

void loadMasterFile(char *fname) {

	FILE *fp;
	long pos = 0;
	char *line, *articulo_id, *stock_min, *p;

	line = malloc(MASTER_REG_SIZE + 2);
	articulo_id = malloc(10);
	stock_min = malloc(10);

	fp = fopen(fname, READ_MODE);

	if (fp == NULL){
		printf("No se pudo abrir el archivo maestro de articulos '%s'.\n", fname);
		exit(1);
	}

	while (fgets(line, MASTER_REG_SIZE + 2, fp) != NULL) {

		articulo_id = substr(line, 1, 8);
		stock_min = substr(line, 49, 5);

		ids_arr[pos] = strtol(articulo_id, &p, 10);
		min_stock_arr[pos] = strtol(stock_min, &p, 10);
		stock_arr[pos] = 0;

		pos++;
	}

	fclose(fp);

	return;
}

long calcularArticulos(char *file_name) {

	FILE *fp;
	long cantreg = 0;

	fp = fopen(file_name, READ_MODE);

	if (fp == NULL) {
		printf("No se pudo abrir el archivo maestro de articulos '%s'.\n", file_name);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);

	cantreg = ftell(fp);
	
	fclose(fp);

	return cantreg / (MASTER_REG_SIZE + 1);
}

char * substr(char * str, int pos, int len) {

	char *aux;
	int c;

	aux = malloc(len + 1);

	if (aux == NULL) {
		printf("No se pudo alocar memoria para el puntero auxiliar.\n");
		exit(1);
	}

	for (c = 0; c < len; c++) {
		*(aux + c) = *(str + pos - 1);
		str++;
	}

	*(aux + c) = '\0';

	return aux;
}

void printHelp() {

	printf("\n\nEste proceso calculara el stock consolidado de las sucursales y generara el pedido de compras\n" 
			"utilizando el archivo maestro de articulos.\n"
			"\nPara poder ejecutar este proceso es necesario indicarle por parametros la cantidad de threads a crear\n"
			"para procesar los archivos de las sucursales y el directorio donde se encuentran los archivos de stock de sucursales\n"
			"y el archivo maestro (deben estar en el mismo directorio).\n"
		    "\nLos archivos de salida se van a guardar en la carpeta 'output' con el siguiente formato:\n\n"
			"\t\t- stock_consolidado \tarchivo de stock consolidado de todas las sucursales\n"
			"\t\t- pedido_compras \tarchivo con los productos y cantidades a comprar basandose en el stock minimo del archivo maestro\n"
			"\t\t- log \t\t\tarchivo de log con detalles de ejecucion como por ejemplo que thread ejecuto que archivo\n"
			"\nEjemplo de llamada:\n\n"
    		"\t./ejercicio2 5 'home/user/directorio/archivos'\n\n");
}

/* EOF */
