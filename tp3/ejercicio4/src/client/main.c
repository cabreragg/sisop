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
#include <netdb.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../constants.h"

void printHelp();
void printMenu();
void handleConnection(int);
int crearMulta(int);
int listarRegistrosSuspender(int);
int ingresarPatente(char *);
void ingresarMonto(char *);

int main(int argc, char *argv[]) {

    int c = getopt(argc, argv, "hH");
    
    switch(c) {
        case 'h':
        case 'H':
            printHelp();
            return 0;
        case -1:
            break;
        default:
            printf("Error en la llamada. Intente './client -help' para mas informacion.\n");
            return 1;
    }

    if (argc != 3) {
        printf("Error en la llamada. Intente './client -help' para mas informacion.\n");
        return 1;
    }

    int err, sd;
    char buffer[256], * hostname, * partido;
    struct addrinfo hints, *addrs;
    
    hostname = argv[1];
    partido = argv[2];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    err = getaddrinfo(hostname, PORT, &hints, &addrs);
    if (err != 0) {
        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(err));
        return 1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        printf("Error al crear el socket.\n");
        return 1;
    }

    struct addrinfo * addr;

    for (addr = addrs; addr != NULL; addr = addr->ai_next) {
        getnameinfo(addr->ai_addr, addr->ai_addrlen, buffer, sizeof(buffer), NULL, 0, NI_NUMERICHOST);

        if (connect(sd, addr->ai_addr, addr->ai_addrlen) == -1) {
            close(sd);
            continue;
        }

        break;
    }
    
    if(addr == NULL) {
        printf("No se pudo conectar con el servidor '%s'.\n", buffer);
        return 1;
    }

    int written = write(sd, partido, strlen(partido));
    
    if (written > 0) {
        handleConnection(sd);
    }

    close(sd);
    freeaddrinfo(addrs);

    return 0;
}

void handleConnection(int sd) {

    int read_size, opcode;
    char buffer[2000];

    printf(CLS);

    read_size = read(sd, buffer, sizeof(buffer));
    buffer[read_size] = '\0';

    printf("%s", buffer);
    printf("\n\n\nPresione 'Enter' para continuar!\n");
    getchar();

    do {
        printMenu();
        scanf("%d", &opcode);
        getchar();

        switch(opcode){
            case 1:
                crearMulta(sd);
                break;
            case 2:
                listarRegistrosSuspender(sd);
                break;
            case 3:
            case 4:
            case 5:
                continue;
            case 6:
                break;
            default:
                printf("La opcion ingresada es invalida.\n"
                        "Presione 'Enter' para continuar.");
                getchar();
                continue;
        }
    } while (opcode != 6);
}

int crearMulta(int sd) {
    
    char patente[20], monto[10], buffer[40], titular[100];
    int read_size = 0, written_size = 0;

    if (ingresarPatente(patente)) {
        return 1;
    }

    ingresarMonto(monto);

    snprintf(buffer, sizeof(buffer), "%s %s %s", "1", patente, monto);

    written_size = write(sd, buffer, strlen(buffer));

    if (!written_size > 0) {
        printf("No se pudo enviar la informacion al server.\n");
        printf("\n\nPresione 'Enter' para continuar.");
        getchar();

        return 1;
    }

    read_size = read(sd, buffer, sizeof(buffer));
    buffer[read_size] = '\0';

    if (buffer == TITULAR) {
        printf("La patente no existe en la base de datos del partido.\n"
                "Ingrese el nombre del titular para poder darla de alta:\n\t");

        fgets(titular, sizeof(titular), stdin);
        titular[strcspn(titular, "\n")] = 0;

        written_size = write(sd, titular, strlen(titular));

        if (!written_size > 0) {
            printf("No se pudo enviar la informacion al server.\n");
            printf("\n\nPresione 'Enter' para continuar.");
            getchar();
            return 1;
        }

        memset(buffer, 0, sizeof(buffer));
        read_size = read(sd, buffer, sizeof(buffer));
    } 

    if (buffer == OK) {
        printf("La multa fue dada de alta exitosamente!\n"
                "Utilice la opcion 'Buscar por patente' del menu principal\n"
                "para verificar la actualizacion de los datos.\n");
        printf("\n\nPresione 'Enter' para continuar.");
        getchar();
    } else {
        printf("La multa no se pudo dar de alta correctamente por error en el server.\n");
        printf("\n\nPresione 'Enter' para continuar.");
        getchar();
    }

    return 0;
}

int listarRegistrosSuspender(int sd) {

    write(sd, "2", 2);
}

void ingresarMonto(char * monto) {
    
    char * p;
    int valido = 0;

    while (!valido) {
        printf(CLS);
        printf("\nIngrese el monto a pagar de la multa:\t");
        
        fgets(monto, sizeof(monto), stdin);
        monto[strcspn(monto, "\n")] = 0;

        long monto_l = strtol(monto, &p, 10);

        if (!(*p == '\n' || *p == '\0')) {
            printf("\nEl monto ingresado '%s' es invalido.\n", monto);
            printf("\n\nPresione 'Enter' para continuar.");
            getchar();
            continue;
        }

        valido = 1;
    }
}

int ingresarPatente(char * patente) {

    regex_t regexp;

    int reti = regcomp(&regexp, "^[a-zA-Z]{2}[0-9]{3}[a-zA-Z]{2}$|^[a-zA-Z]{3}[0-9]{3}$", REG_EXTENDED);
    
    if (reti) {
        printf("No se puede validar la patente.\n"
                "\n\nPresione 'Enter' para continuar.");
        getchar();
        return 1;
    }

    do {
        printf(CLS);
        printf("\nIngrese la patente a la que desea cargarle la multa:\t");

        //Si hago sizeof(patente) me trae el tamanio del puntero = 8
        fgets(patente, 20, stdin);
        patente[strcspn(patente, "\n")] = 0;
        
        reti = regexec(&regexp, patente, 0, NULL, 0);

        if (reti) {
            printf("\nLa patente ingresada '%s' es invalida, por favor intente nuevamente.\n", patente);
            printf("\n\nPresione 'Enter' para continuar.");
            getchar();
            return 1;
        }

    } while(reti != 0);

    return 0;
}

void printMenu() {

    printf(CLS);
    printf("\nSeleccione una operacion:\n\n"
            "\t\t1. Ingresar nueva multa.\n"
            "\t\t2. Listar registros a suspender.\n"
            "\t\t3. Cancelar multas.\n"
            "\t\t4. Buscar por patente.\n"
            "\t\t5. Ver monto total de infractores.\n"
            "\t\t6. Salir.\n\n");
}

void printHelp() {

    printf("Bienvenido al CLIENTE\n\n\n\n\n");
}

/* EOF */