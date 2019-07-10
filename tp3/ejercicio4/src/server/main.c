/*
Ejercicio4: main.c
TP3 - Ejercicio 4

Integrantes:
    Bouza Facundo 	DNI 41332191
    Cabrera Gonzalo	DNI 37350600
    Facal Ernesto 	DNI 38983722
    Marson Tomás	DNI 40808276
    Viescas Julián	DNI 40258471

Número de entrega: primera reentrega.
*/

#define _XOPEN_SOURCE 600

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../constants.h"

void printHelp();
void * handleConnection(void *);
int crearMulta(int, char *, char *);

int main(int argc, char *argv[]) {

    int c = getopt(argc, argv, "hH");
    
    switch(c) {
        case 'h':
        case 'H':
            printHelp();
            exit(0);
        case -1:
            break;
        default:
            printf("Error en la llamada. Intente './server -help' para mas informacion.\n");
            exit(1);
    }

    if (argc > 1) {
        printf("Error en la llamada. Intente './server -help' para mas informacion.\n");
        exit(1);
    }

    int se;
    struct addrinfo hints, *addrs;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &addrs);

    se = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    
    if (se == -1) {
        printf("Error creando el socket.\n");
        exit(1);
    }

    if (bind(se, addrs->ai_addr, addrs->ai_addrlen) == -1) { 
        printf ("Error bindeando el socket.\n");
        exit(1); 
    }

    if (listen (se, MAX_CONNECTIONS) == -1) { 
        printf ("Error al intentar recibir peticiones de conexion.\n");
        exit(1);
    }

    struct sockaddr client; 
    int sc, client_len, cant_threads = 0; 
    pthread_t thread_id[MAX_CONNECTIONS];
    char hostname[256];

    client_len = sizeof((struct sockaddr *) &client);

    while (sc = accept(se, &client, &client_len)) {
        if (sc < 0) {
            printf("Error al intentar aceptar la conexion: '%s'\n",strerror(errno));
            continue;
        }
        
        getnameinfo(&client, client_len, hostname, sizeof(hostname), NULL, 0, 0);

        printf("Conexion establecida con el cliente '%s'.\n", hostname);
         
        if (pthread_create(&thread_id[cant_threads], NULL, handleConnection, (void*)&sc) < 0) {
            printf("No se pudo crear el thread de conexion.\n");
            exit(1);
        }
         
        printf("Se creo exitosamente el thread de conexion.\n");

        cant_threads++;

        if (cant_threads == MAX_CONNECTIONS - 2) {
            pthread_join(thread_id[cant_threads], NULL);
            
            cant_threads--;
        }
    }

    close(se);

    return 0;
}

void * handleConnection(void * socket_desc) {

    int sc = *(int * )socket_desc;
    int read_size = 0, written_size = 0;
    char * message, partido[100], client_message[2000];

    read_size = read(sc, partido, sizeof(partido));

    partido[read_size] = '\0';

    message = "\nBIENVENIDO AL SISTEMA DE MULTAS DE LA PROVINCIA DE BUENOS AIRES!\n\n\nEl partido ingresado es: ";
    
    memset(client_message, 0, sizeof(client_message));
    strcat(client_message, message);
    strcat(client_message, partido);

    written_size = write(sc, client_message, strlen(client_message));
    
    memset(client_message, 0, sizeof(client_message));

    while((read_size = read(sc, client_message, sizeof(client_message))) > 0) {

        char *p, *t, *opcode;
        
        opcode = strtok_r(client_message, " ", &t);
        long opcode_l = strtol(opcode, &p, 10);
        
        switch(opcode_l){
            case 1:
                crearMulta(sc, partido, t);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
        }
    }

    close(sc);
    return 0;
}

int crearMulta(int sc, char *partido, char *mensaje) {

    FILE * multa_ptr = NULL;

    char fileName[100] = "/home/gonza/Desktop/TP3/ejercicio4/output/";
    char line[256] ="ASD"; 

    strcat(fileName, partido);

    multa_ptr = fopen(fileName, READ_WRITE);

    if (multa_ptr == NULL) {
        write(sc, NOT_OK, strlen(NOT_OK));
    }

    char * patente, *monto;

    patente = strtok_r(mensaje, " ", &monto);

    while (fgets(line, sizeof(line), multa_ptr) != NULL) {        
        
        if(strncmp(line, patente, strlen(patente)) == 0) {
            break;
        }

    }

    printf("LINE %s\n", line);

    fclose(multa_ptr);

}

void printHelp() {

    printf("\n\nBienvenido al servidor del sistema de multas de la Provincia de Buenos Aires!\n\n\n"
            "Este server va a escuchar peticiones de los clientes en el puerto indicado en el archivo de constantes 'constants.c'.\n"
            "\nLas operaciones disponibles son:\n"
            "\t\t1. Ingresar nueva multa.\n"
            "\t\t2. Listar registros a suspender.\n"
            "\t\t3. Cancelar multas.\n"
            "\t\t4. Buscar por patente.\n"
            "\t\t5. Ver monto total de infractores.\n\n"
            "Para ejecutar el server, simplemente ingrese './server'\n\n");
}

/* EOF */