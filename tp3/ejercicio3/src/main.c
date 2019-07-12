/*
Ejercicio3: main.c
TP3 - Ejercicio 3

Integrantes:
    Bouza Facundo 	DNI 41332191
    Cabrera Gonzalo	DNI 37350600
    Facal Ernesto 	DNI 38983722
    Marson Tomás	DNI 40808276

Número de entrega: primera reentrega.
*/

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "constants.h"
#include "structs.h"

void printHelp();
void ticketDataToString(ticket_data, char []);
void createFileName(char [], char [], char []);
FILE * openFile(int);
int saveToFile(char []);

char * dname;

int main(int argc, char *argv[]){

    if (argc == 1) {
        printf("Error en la llamada. Intente './ejercicio3 -help' para mas informacion.\n");
        return 1;
    }

    int c = getopt(argc, argv, "hH");
    
    switch(c) {
        case 'h':
        case 'H':
            printHelp();
            return 0;
            break;
        case -1:
            break;
        default:
            printf("Error en la llamada. Intente './ejercicio3 -help' para mas informacion.\n");
            return 1;
    }

    char * myfifo = argv[1];
    FILE * fd = NULL;
    char line[100];

    dname = dirname(argv[0]);

    int pid = fork();

    if (pid) {
        printf("Proceso central creado correctamente. PID:%d \n", pid);
        return 0;
    }

    fd = fopen(myfifo, READ_ONLY);

    if (fd == NULL) {
        printf("No se pudo abrir el FIFO '%s', intentando crearlo...\n", myfifo);

        if(mkfifo (myfifo, 0666)) {
            printf("No se pudo crear el FIFO '%s'. Verifique el PATH e intente nuevamente.\n", myfifo);
            return 1;
        }

        printf("Se creo correctamente el FIFO '%s', envie mensaje para comenzar a procesar...\n", myfifo);
        
        fd = fopen(myfifo, READ_ONLY);
    }

    while(1) {
        int leidos = fread(&line, 1, sizeof(line), fd);
        
        line[leidos-1] = '\0';

        if(leidos){
            saveToFile(line);
        }
    }
}

int saveToFile(char line[]){

    FILE * dump_ptr = NULL;
    FILE * ticket_ptr = NULL;

    cam_data data;
    
    if ((sscanf(line, "%7s %4s %d %4s", data.plate, data.cam, &data.speed, data.unit)) != 4) {
        printf("No se pudo parsear correctamente el mensaje '%s'. Por favor, revisar el formato.\n"
                "Continuando con el siguiente mensaje...\n", line);

        return 1;
    }

    printf("%s\n", line);

    dump_ptr = openFile(DUMP_FILE);
    
    if(dump_ptr != NULL){
        fputs(line, dump_ptr);
        fputs("\n", dump_ptr);
        fclose(dump_ptr);
    }

    if(TICKET(data.speed)){

        time_t now = time(&now);
        struct tm * time = localtime(&now);

        ticket_data tdata;

        tdata.cam_data = data;
        tdata.amount = TICKET_AMOUNT(data.speed);
        
        strftime(tdata.date, 11, "%d/%m/%Y", time);
        strftime(tdata.hora, 6, "%H:%M", time);

        char tDataString[50];

        ticket_ptr = openFile(TICKET_FILE);

        if(ticket_ptr != NULL){
            ticketDataToString(tdata, tDataString);
            fputs(tDataString, ticket_ptr);
            fputs("\n", ticket_ptr);
            fclose(ticket_ptr);
        }
    }

    return 0;
}

void ticketDataToString(ticket_data data, char str[50]){

    snprintf(str, 50, "%s %s %s %d %s $%d %s", data.cam_data.plate, data.hora, data.date, data.cam_data.speed, data.cam_data.unit, data.amount, "\0");
}

void createFileName(char file[25], char outdir[256], char outputName[256]){

    time_t now = time(&now);
    struct tm * time = localtime(&now);

    char buffer[9];

    strftime(buffer, 9, "%Y%m%d", time);

    snprintf(outputName, 256, "%s%s%s%s", outdir, file, buffer, ".txt\0");
}

FILE * openFile(int fileToCreate){

    FILE * file_ptr = NULL;
    char outputFile[256], outdir[256];

    snprintf(outdir, 100, "%s%s", dname, "/output");
    mkdir(outdir, 0700);
    
    switch(fileToCreate){
        case DUMP_FILE: 
            createFileName("/Transito_", outdir, outputFile);
            break;
        case TICKET_FILE:
            createFileName("/Crear_Multas_", outdir, outputFile);
            break;
        default: 
            return NULL;
    }

    file_ptr = fopen(outputFile, "a");
    
    if(file_ptr == NULL) {
        printf("Error at opening file '%s'.\n", outputFile);
        return file_ptr;
    }

    return file_ptr;
}

void printHelp(){

    printf("\n\nBienvenido al sistema centralizado de multas de la provincia de Buenos Aires.\n\n"
            "Este sistema registrara toda la informacion recibida de las camaras de velocidad distribuidas\n"
            "por la provincia y calculara, en los casos que corresponda, el monto de las multas\n"
            "debidas a exceso de velocidad de los automovilistas.\n\n"
            "Para poder operar correctamente, el proceso debe recibir como argumento el nombre de\n"
            "la estructura FIFO utilizado por las camaras para transmitir dicha informacion.\n"
            "Se intentara abrir la estructura y comenzar a leer, de no existir, se procedera a crearla.\n\n"
            "Recuerde que de no existir/no haber mensajes en el FIFO, el proceso quedara bloqueado hasta\n"
            "que el primer mensaje sea depositado.\n\n"
            "Todas las salidas (archivo completo de informacion recibida y archivo de multas calculadas)\n"
            "se guardaran dentro del directorio 'output' con el siguiente formato:\n\n"
            "Archivo de mensajes recibidos:\n"
            "\t\t'Transito_YYYYMMDD.txt'\n\n"
            "Archivo de multas calculadas:\n"
            "\t\t'Crear_Multas_YYYYMMDD.txt'\n\n"
            "Ejemplo de llamada:\n\n"
            "\t\t./ejercicio3 \"/home/user/FIFO\"\n\n\n");
}

//EOF