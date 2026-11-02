#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>


//para compilarlo es necesario poner: gcc -Wall -Wextra -o ej3ia ej3ia.c -lm -pthread


#define FICHERO "resultados.txt"
#define SEM_NAME "/sem_entregable3"

double ahora(void) {                       //funcion creada para ver el tiempo actual en el momento de la llamada
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

double suma(long ini, long fin) {          //funcion que nos hace la operacion pedida
    double s = 0.0;
    for (long i = ini; i <= fin; i++)
        s += tan(sqrt((double)i));
    return s;
}

int main(int argc, char *argv[]) {
    if (argc < 3) { fprintf(stderr, "Uso: %s N P\n", argv[0]); return 1; }//si no se introducen los valores al ejecutar nos echa del programa
    long N = atol(argv[1]);
    int  P = atoi(argv[2]);
    if (N <= 0 || P <= 0) { fprintf(stderr, "N y P > 0\n"); return 1; }//si alguno de los numeros introducidos es negativo da error y sale del programa

    fclose(fopen(FICHERO, "w"));           //creamos el documento




    /*
    * Semáforo con nombre (sem_unlink + sem_open):
    *   - sem_unlink borra un semáforo anterior con el mismo nombre, por si
    *     quedó de una ejecución interrumpida, para empezar siempre limpio.
    *   - sem_open crea el semáforo con valor inicial 1 (puerta libre).
    *     Los hijos usan sem_wait antes de escribir en el fichero y sem_post
    *     al terminar, de modo que solo uno escribe a la vez y las líneas
    *     no se mezclan.
    */
    sem_unlink(SEM_NAME);              
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    // 0: indica base octal; 6: lectura/escritura (dueño); 44: solo lectura (grupo y otros)

    long tam =N/P, resto =N%P;

    //creamos los procesos hijos
    for (int i = 0; i < P; i++) {
        if (fork() == 0) {
            double t0 = ahora();
            long ini, fin;
            if (i == 0) { 
                ini = 1; 
                fin = tam+resto; 
            }
            else { 
                ini = i*tam+resto+1;
                fin = (i+1)*tam+resto; 
            }

            double s = suma(ini, fin);
            double t = ahora() - t0;

            printf("[Hijo %d] suma=%.15e tiempo=%.6f s\n", i + 1, s, t);

            sem_wait(sem);                 //los hijos entran a escribir
            FILE *f = fopen(FICHERO, "a"); // "a" = append
            fprintf(f, "PARCIAL %d %.15e %.6f\n", i + 1, s, t);
            fclose(f);
            sem_post(sem);                 //los hijos salen del fichero
            exit(0);
        }
    }
    for (int i = 0; i < P; i++) wait(NULL);          //aqui se espera a los hijos
    // wait(NULL): espera a que termine un hijo ignorando su estado de salida

    //aqui hace su proceso el hijo p+1
    if (fork() == 0) {
        double t0 = ahora();//se coge el tiempo actual

        //abre el archivo y va sumando los resultados de los hermanos para hacer el calculo de la media
        FILE *f = fopen(FICHERO, "r");
        char tipo[20]; int id; double valor, t, total = 0;
        while (fscanf(f, "%s %d %lf %lf", tipo, &id, &valor, &t) == 4)
            total += valor;
        fclose(f);
        double media = total / N;
        double tt = ahora() - t0;//se calcula la diferencia de tiempo
        
        //hace las impresiones
        printf("[Hijo P+1] media=%.15e tiempo=%.6f s\n", media, tt);
        f = fopen(FICHERO, "a");
        fprintf(f, "FINAL_P1 %d %.15e %.6f\n", P + 1, media, tt);
        fclose(f);
        exit(0);
    }
    wait(NULL);//aqui se espera a que acabe el hijo 1

    //aqui hace su proceso el hijo p+2
    if (fork() == 0) {
        double t0 = ahora();//se inicia el tiempo
        double media = suma(1, N) / N;//se hacen todas las operaciones
        double tt = ahora() - t0;//se calcula la diferencia de tiempo
        printf("[Hijo P+2] media=%.15e tiempo=%.6f s\n", media, tt);//se hacen las impresiones por pantalla y en el fichero
        FILE *f = fopen(FICHERO, "a");
        fprintf(f, "FINAL_P2 %d %.15e %.6f\n", P + 2, media, tt);
        fclose(f);
        exit(0);
    }
    wait(NULL);//aqui se espera a que acabe el hijo 2

    //aqui el padre hace las impresiones y las operaciones necesarias para ver las diferencias entre los procesos
    FILE *f = fopen(FICHERO, "r");
    char tipo[20]; int id; double valor, time;
    double m1 = 0, m2 = 0;
    printf("\n===== RESUMEN =====\n");
    while (fscanf(f, "%s %d %lf %lf", tipo, &id, &valor, &time) == 4) {
        printf("Hijo %d (%s): %.6f s\n", id, tipo, time);
        if (strcmp(tipo, "FINAL_P1") == 0) m1 = valor;
        if (strcmp(tipo, "FINAL_P2") == 0) m2 = valor;
    }
    fclose(f);                                          //aqui se cierra el fichero
    printf("Media P+1: %.15e\nMedia P+2: %.15e\n", m1, m2);//aqui se ponen las medias resultantes
    printf("Diferencia: %.15e\n", fabs(m1 - m2));//aqui se calcula e imprime por pantalla la diferencia


    //en estas dos lineas se cierra el uso del semaforo
    sem_close(sem);      
    sem_unlink(SEM_NAME);
    return 0;
}


/*
 * CONCLUSIONES
 *
 * Ejecución realizada: ./ej3 100000000 4  
 *
 * Resultados:
 *   - Hijos 1-4 (concurrentes): 0.513 s, 0.518 s, 0.532 s y 0.536 s
 *   - Hijo P+1 (reducción):     0.0001 s
 *   - Hijo P+2 (secuencial):    2.056 s
 *   - Media P+1: -9.782737496463619e-01
 *   - Media P+2: -9.782737496459517e-01
 *   - Diferencia: 4.1e-13
 *
 * - Corrección: las medias de P+1 (distribuida) y P+2 (secuencial) coinciden
 *   en 12 cifras. La diferencia se debe a que la suma en coma flotante no
 *   es asociativa, no a un error de reparto.
 *
 * - Rendimiento: el hijo más lento tardó 0.536 s frente a 2.056 s del
 *   cálculo secuencial: speedup ~3.83 y eficiencia ~96 % (máximo teórico: 4).
 *   Los cuatro hijos tardan casi lo mismo, así que la carga está bien
 *   equilibrada.
 *
 * - Reducción: el hijo P+1 solo lee P líneas y suma P valores, por lo que su
 *   coste es despreciable frente al cálculo.
 *
 * - Sincronización: el semáforo evita que los hijos mezclen sus líneas en el
 *   fichero, y las etiquetas (PARCIAL, FINAL_P1, FINAL_P2) identifican a cada
 *   escritor.
 */