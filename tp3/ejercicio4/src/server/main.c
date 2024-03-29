/*
Ejercicio4: main.c
TP3 - Ejercicio 4

Integrantes:
    Bouza Facundo 	DNI 41332191
    Cabrera Gonzalo	DNI 37350600
    Facal Ernesto 	DNI 38983722
    Marson Tomás	DNI 40808276
	Viescas Julián	DNI 40258471

Número de entrega: segunda reentrega.
*/

#define _XOPEN_SOURCE 600

#include <arpa/inet.h>
#include <errno.h>
#include <libgen.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../constants.h"

void * handleConnection(void *);
void closeConnection(int);
void printHelp();
void crearMulta(int, char *, char *);
void listarRegistrosSuspender(int, char *);
void borrarMulta(int, char *, char *);
void buscarPatente(int, char *, char *);
void listarMontoTotal(int, char *);

char outdir[256];
pthread_rwlock_t data_file, tmp_file;

int main(int argc, char *argv[]) {

    signal(SIGINT, closeConnection);
    signal(SIGPIPE, closeConnection);
    signal(SIGSEGV, closeConnection);

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
    char * dname;

    dname = dirname(argv[0]);

    snprintf(outdir, 256, "%s%s", dname, "/data/");
    mkdir(outdir, 0700);

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

    pthread_rwlock_init(&data_file, NULL);
    pthread_rwlock_init(&tmp_file, NULL);

    while (sc = accept(se, &client, &client_len)) {
        if (sc < 0) {
            printf("Error al intentar aceptar la conexion: '%s'\n",strerror(errno));
            continue;
        }
        
        getnameinfo(&client, client_len, hostname, sizeof(hostname), NULL, 0, 0);

        printf("Conexion establecida con el cliente '%s'.\n", hostname);
         
        if (pthread_create(&thread_id[cant_threads], NULL, handleConnection, &sc) < 0) {
            printf("No se pudo crear el thread de conexion.\n");
            exit(1);
        }
         
        printf("Se creo exitosamente el thread de conexion.\n");

        cant_threads++;

        if (cant_threads == MAX_CONNECTIONS) {

            for(cant_threads; cant_threads >= MAX_CONNECTIONS; cant_threads--) {
                pthread_join(thread_id[cant_threads-1], NULL);
                cant_threads--;
            }
        }
    }

    close(se);

    return 0;
}

void closeConnection(int sig) {

    printf("Se recibio la SIGNAL: '%d'. Cerrando proceso y liberando recursos...\n", sig);
    exit(0);
}

void * handleConnection(void * socket_desc) {

    int sc = *(int *)socket_desc;
    int read_size = 0, written_size = 0;
    char * message, partido[100], client_message[2000];

    read_size = read(sc, partido, sizeof(partido));

    partido[read_size] = '\0';

    message = "BIENVENIDO AL SISTEMA DE MULTAS DE LA PROVINCIA DE BUENOS AIRES!\n\n\nEl partido ingresado es: ";
    
    memset(client_message, 0, sizeof(client_message));
    strcat(client_message, message);
    strcat(client_message, partido);

    written_size = write(sc, client_message, strlen(client_message));
    
    memset(client_message, 0, sizeof(client_message));

    while ((read_size = read(sc, client_message, sizeof(client_message))) > 0) {

        char *p, *token, *opcode;
        
        opcode = strtok_r(client_message, TOKEN, &token);
        long opcode_l = strtol(opcode, &p, 10);
        
        switch (opcode_l) {
            case 1:
                crearMulta(sc, partido, token);
                break;
            case 2:
                listarRegistrosSuspender(sc, partido);
                break;
            case 3:
                borrarMulta(sc, partido, token);
                break;
            case 4:
                buscarPatente(sc, partido, token);
                break;
            case 5:
                listarMontoTotal(sc, partido);
                break;
        }

        memset(client_message, 0, sizeof(client_message));
    }

    close(sc);
    return 0;
}

void crearMulta(int sc, char *partido, char *message) {

    FILE * ticket_ptr = NULL, *tmp_ptr = NULL;
    char fileName[256], tmpName[256], line[256], new_line[256];
    char * plate, *amount, *amount_line, *holder, titular[100], *p, *fines;
    int result = -1, read_size, written_size, pos = 0, pos2 = 0;
    long l_amount, l_amount_line, l_fines;

    snprintf(fileName, sizeof(fileName), "%s%s", outdir, partido);

    ticket_ptr = fopen(fileName, READ_WRITE);

    if (ticket_ptr == NULL) {
        ticket_ptr = fopen(fileName, WRITE);
    }

    if (ticket_ptr == NULL) {
        write(sc, NOT_OK, strlen(NOT_OK));
        return;
    }

    plate = strtok_r(message, TOKEN, &amount);

    pthread_rwlock_rdlock(&data_file);

    while (fgets(line, sizeof(line), ticket_ptr) != NULL) {        

        if ((result = strncmp(line, plate, strlen(plate))) == 0) {
            break;
        }

        pos++;
    }

    pthread_rwlock_unlock(&data_file);

    if (!result) {

        l_amount = strtol(amount, &p, 10);

        strtok_r(line, TOKEN, &p);
        holder = strtok_r(NULL, TOKEN, &p);
        fines = strtok_r(NULL, TOKEN, &p);
        amount_line = strtok_r(NULL, TOKEN, &p);

        l_fines = strtol(fines, &p, 10);        
        l_amount_line = strtol(amount_line, &p, 10);

        l_fines++;
        l_amount_line += l_amount;

        snprintf(new_line, sizeof(new_line), "%s,%s,%ld,%ld%s", plate, holder, l_fines, l_amount_line, "\n");

        snprintf(tmpName, sizeof(tmpName), "%s%s", outdir, "temp");

        pthread_rwlock_wrlock(&tmp_file);

        tmp_ptr = fopen(tmpName, WRITE);

        if (tmp_ptr == NULL) {
            write(sc, NOT_OK, strlen(NOT_OK));
            return;
        }

        fseek(ticket_ptr, 0l, SEEK_SET);

        pthread_rwlock_rdlock(&data_file);

        while (fgets(line, sizeof(line), ticket_ptr) != NULL) {

            if (pos != pos2) {
                written_size = fwrite(line, 1, strlen(line), tmp_ptr);
            } else {
                written_size = fwrite(new_line, 1, strlen(new_line), tmp_ptr);
            }

            pos2++;
        }

        pthread_rwlock_unlock(&tmp_file);
        pthread_rwlock_unlock(&data_file);

        fclose(ticket_ptr);
        fclose(tmp_ptr);

        remove(fileName);
        rename(tmpName, fileName);

        write(sc, OK, strlen(OK));
    } else {
        write(sc, TITULAR, strlen(TITULAR));

        read_size = read(sc, titular, sizeof(titular));
        
        if (!read_size) {
            return;
        }

        titular[read_size] = '\0';

        snprintf(line, sizeof(line), "%s,%s,%s,%s%s", plate, titular, "1", amount, "\n");

        pthread_rwlock_wrlock(&data_file);

        written_size = fwrite(line, 1, strlen(line), ticket_ptr);

        pthread_rwlock_unlock(&data_file);

        fclose(ticket_ptr);

        if (written_size > 0) {
            write(sc, OK, strlen(OK));
        } else {
            write(sc, NOT_OK, strlen(NOT_OK));
        }
    }

    return;
}

void listarRegistrosSuspender(int sc, char *partido) {

    FILE * ticket_ptr = NULL;
    char fileName[256], line[256];
    char *amount, *fines, *p, buffer[256];
    int read_size, written_size;
    long l_amount, l_fines;

    snprintf(fileName, sizeof(fileName), "%s%s", outdir, partido);

    ticket_ptr = fopen(fileName, READ);

    if (ticket_ptr == NULL) {
        write(sc, NOT_OK, strlen(NOT_OK));
        return;
    }

    pthread_rwlock_rdlock(&data_file);

    while (fgets(line, sizeof(line), ticket_ptr) != NULL) {        

        strcpy(buffer, line);

        strtok_r(buffer, TOKEN, &fines);
        strtok_r(NULL, TOKEN, &fines);

        l_fines = strtol(fines, &p, 10);

        amount = strrchr(line, TOKEN_I);
        amount++;

        l_amount = strtol(amount, &p, 10);

        if (l_amount > MAX_TOTAL_AMOUNT || l_fines > MAX_AMOUNT_FINES) {
            written_size = write(sc, line, strlen(line));
        }
    }

    pthread_rwlock_unlock(&data_file);

    written_size = write(sc, DONE, strlen(DONE));

    fclose(ticket_ptr);

    return;
}

void borrarMulta(int sc, char *partido, char *message) {

    FILE * ticket_ptr = NULL, *tmp_ptr = NULL;
    char fileName[256], tempName[256], line[256];
    int pos = 0, pos2 = 0, result = -1, written_size;

    snprintf(fileName, sizeof(fileName), "%s%s", outdir, partido);

    ticket_ptr = fopen(fileName, READ);

    if (ticket_ptr == NULL) {
        write(sc, NOT_OK, strlen(NOT_OK));
        return;
    }

    pthread_rwlock_rdlock(&data_file);

    while (fgets(line, sizeof(line), ticket_ptr) != NULL) {

        if ((result = strncmp(line, message, strlen(message))) == 0) {
            break;
        }

        pos++;
    }

    pthread_rwlock_unlock(&data_file);

    if (!result) {
        snprintf(tempName, sizeof(tempName), "%s%s", outdir, "temp");

        tmp_ptr = fopen(tempName, WRITE);

        if (tmp_ptr == NULL) {
            write(sc, NOT_OK, strlen(NOT_OK));
            return;
        }

        fseek(ticket_ptr, 0l, SEEK_SET);

        pthread_rwlock_rdlock(&data_file);
        pthread_rwlock_wrlock(&tmp_file);

        while (fgets(line, sizeof(line), ticket_ptr) != NULL) {

            if (pos != pos2) {
                written_size = fwrite(line, 1, strlen(line), tmp_ptr);
            }

            pos2++;
        }

        pthread_rwlock_unlock(&data_file);
        pthread_rwlock_unlock(&tmp_file);

        fclose(ticket_ptr);
        fclose(tmp_ptr);

        remove(fileName);
        rename(tempName, fileName);

        write(sc, OK, strlen(OK));
    } else {
        write(sc, NOT_FOUND, strlen(NOT_FOUND));
        fclose(ticket_ptr);
    }

    return;
}

void buscarPatente(int sc, char *partido, char *message) {

    FILE * ticket_ptr = NULL;
    char fileName[256], line[256];
    int result = -1;

    snprintf(fileName, sizeof(fileName), "%s%s", outdir, partido);

    ticket_ptr = fopen(fileName, READ);

    if (ticket_ptr == NULL) {
        write(sc, NOT_OK, strlen(NOT_OK));
        return;
    }

    pthread_rwlock_rdlock(&data_file);

    while (fgets(line, sizeof(line), ticket_ptr) != NULL) {        
        if ((result = strncmp(line, message, strlen(message))) == 0) {
            break;
        }
    }

    pthread_rwlock_unlock(&data_file);

    fclose(ticket_ptr);

    if (!result) {   
        write(sc, line, strlen(line));
        return;
    }

    write(sc, NOT_FOUND, strlen(NOT_FOUND));
    return;
}

void listarMontoTotal(int sc, char *partido) {

    FILE * ticket_ptr = NULL;
    char fileName[256], line[256], *amount, *p, buffer[100];
    long l_amount, total = 0;

    snprintf(fileName, sizeof(fileName), "%s%s", outdir, partido);

    ticket_ptr = fopen(fileName, READ);

    if (ticket_ptr == NULL) {
        write(sc, NOT_FOUND, strlen(NOT_FOUND));
        return;
    }

    pthread_rwlock_rdlock(&data_file);

    while (fgets(line, sizeof(line), ticket_ptr) != NULL) {        
        
        amount = strrchr(line, TOKEN_I);
        amount++;
        
        l_amount = strtol(amount, &p, 10);

        total += l_amount;
    }

    pthread_rwlock_unlock(&data_file);

    if (total <= 0) {

        fseek(ticket_ptr, 0l, SEEK_END);

        if (ftell(ticket_ptr)) {
            write(sc, NOT_OK, strlen(NOT_OK));
        } else {
            write(sc, NOT_FOUND, strlen(NOT_FOUND));
        }
        
        fclose(ticket_ptr);
        return;
    }

    fclose(ticket_ptr);

    snprintf(buffer, sizeof(buffer), "%s,%ld", partido, total);
    write(sc, buffer, strlen(buffer));

    return;
}

void printHelp() {

    printf("\n\nBienvenido al servidor del sistema de multas de la Provincia de Buenos Aires!\n\n\n"
            "Este server va a escuchar peticiones de los clientes en el puerto indicado en el archivo de constantes 'constants.h'.\n"
            "\nLas operaciones disponibles son:\n"
            "\t\t1. Ingresar nueva multa.\n"
            "\t\t2. Listar registros a suspender.\n"
            "\t\t3. Cancelar multas.\n"
            "\t\t4. Buscar por patente.\n"
            "\t\t5. Ver monto total de infractores.\n\n"
            "Los datos de todas las multas ingresadas seran guardados dentro de la carpeta 'data' en un archivo de texto\n"
            "con el nombre del partido correspondiente.\n\n"
            "Para ejecutar el server, simplemente ingrese './server'\n\n");
}

/* EOF */
