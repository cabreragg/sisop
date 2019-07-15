/*
Ejercicio1: main.c
TP3 - Ejercicio 1

Integrantes:
    Bouza Facundo 	DNI 41332191
    Cabrera Gonzalo	DNI 37350600
    Facal Ernesto 	DNI 38983722
    Marson Tomás	DNI 40808276

Número de entrega: segunda reentrega.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printHelp();
int createDaemons();
int createChildrens();
int createZombies();

int main(int argc, char *argv[]){

    if (argc == 1) {
        printf("Error en la llamada. Intente './ejercicio1 -help' para mas informacion.\n");
        return 1;
    }

    int c = getopt(argc, argv, "hHdDcCzZ");
    
    switch(c) {
        case 'h':
        case 'H':
            printHelp();
            return 0;
        case 'd':
        case 'D':
            createDaemons();
            break;
        case 'c':
        case 'C':
            createChildrens();
        case 'z':
        case 'Z':
            createZombies();
            break;
        case -1:
        default:
            printf("Error en la llamada. Intente './ejercicio3 -help' para mas informacion.\n");
            return 1;
    }

    return 0;
}

int createDaemons() {

    int pid;

    for (int i=0; i<2; i++) {
        pid = fork();

        if (!pid) {
            break;
        }
    }

    if(pid > 0) {
        printf("Se crearon los dos procesos demonio correctamente.\n");
        return 0;
    }

    sleep(1);

    printf("PID: %d PPID: %d Parentesco-Tipo: %s-%s\n", getpid(), getppid(), "hijo", (getppid()==1) ? ("demonio") : ("normal"));

    for (int i = 0; i < 1000000; i++) {
        sleep(2);
    }

    return 0;
}

int createChildrens() {

    int pid;

    for (int i=0; i<10; i++) {
        pid = fork();

        if (!pid) {
            break;
        }
    }

    if (!pid) {
        printf("PID: %d PPID: %d Parentesco-Tipo: %s-%s\n", getpid(), getppid(), "hijo", (getppid()==1) ? ("demonio") : ("normal"));

        for (int i = 0; i < 1000000; i++) {
            sleep(2);
        }
    }
    else {
        sleep(1);
        
        printf("\nOprima cualquier tecla para matar a todos los procesos.\n");
        
        getchar();
        kill(0, SIGKILL);
    }
}

int createZombies() {

    int pid;
    int zombie = 0;

    for (int i=0; i<2; i++) {
        pid = fork();

        if (!pid) {
            break;
        }
    }

    if (pid == 0) {
        printf("PID: %d PPID: %d Parentesco-Tipo: %s-%s\n", getpid(), getppid(), "hijo", (getppid()==1) ? ("demonio") : ("normal"));
        
        for (int i=0; i<3; i++) {
            pid = fork();

            if (pid == 0) {
                break;
            }

            zombie = 1;
        }

        if (pid == 0) {
            /* Soy los nietos */
            if (zombie == 0) {
                printf("PID: %d PPID: %d Parentesco-Tipo: %s-%s\n", getpid(), getppid(), "nietos", "zombie");
                exit(0);
            } else {
                printf("PID: %d PPID: %d Parentesco-Tipo: %s-%s\n", getpid(), getppid(), "nietos", (getppid()==1) ? ("demonio") : ("normal"));
                sleep(2000);
            }   
        } else {
            /* Soy los hijos */
            sleep(3000);
        }
    } else {
        /* Soy el padre */
        sleep(2);
        
        printf("\nOprima cualquier tecla para matar a todos los procesos.\n");
        
        getchar();
        kill(0, SIGKILL);
    }
}

void printHelp() {

    printf("TP3 - Ejercicio 1\n"
            "Este programa puede recibir los siguientes parámetros:\n\n"
            "\t-d\tcrea dos procesos demonios, que ejecutan cada 2 segundos un loop hasta acumular 1.000.000.\n\n"
            "\t-c\tcrea 10 procesos hijos, donde cada uno realiza el mismo procesamiento que en el caso\n"
            "\t\tanterior. El padre se queda esperando a que se presione una tecla y en este momento se\n"
            "\t\tcierran todos los procesos.\n\n"
            "\t-z\tcrea 6 procesos nietos, de dos hijos distintos, y 2 de ellos quedan como zombies.\n"
            "\t\tEl padre se queda esperando a que se presione una tecla para poder verificar la correcta\n"
            "\t\tgeneración de los procesos, y luego cierra los procesos.\n\n"
            "\t-h\timprime este mensaje.\n\n");
}

/* EOF */
