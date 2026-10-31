/*
    Autor: Pazos Morales, Julio, juliojose.pazos@rai.usc.es
    Fecha: 26/10/26
    Versión: 1
*/
// Descripción del programa.

// Todas estas librerías son necesarias.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void gestion_usr1(int);
static void gestion_alrm(int);

// Variables globales en las que se guarda la acción del manejador. Ese tipo de dato va directamente a memoria
//      y su lectura/escritura es en un sólo paso de la CPU.
volatile sig_atomic_t flag_usr1 = 0;
volatile sig_atomic_t flag_alrm = 0;
// Punto de inicio.
int main(void)
{
    // Declaramos la variable que tiene el struct sigaction. Describirá que hacer cuando llegue una señal. Es como un formulario
    //      que se entrega al sistema.
    struct sigaction usr1;
    // Indica qué función ejecutar cuando llegue la señal. Debe tener la forma "void funct(int)", el 'int' es el número de la señal.
    usr1.sa_handler = gestion_usr1;
    // Aquí se establecería qué otras señales bloquear mientras se ejecuta el manejador. La función sigemptyset() lo deja vacío.
    sigemptyset(&usr1.sa_mask);
    // Hace que si la señal interrumpe otra llamada al sistema, que se reanude esa llamada al terminar el manejador, en vez de fallar.
    usr1.sa_flags = SA_RESTART;
    // Aquí es donde se le dice al sistema que para cuando llegue la señal SIGUSR1, que actúe según los parámetros establecidos en
    //      el struct.
    if (sigaction(SIGUSR1, &usr1, NULL) == -1)
        exit(0);

    // El mismo bloque de código para SIGALRM.
    struct sigaction alrm;
    alrm.sa_handler = gestion_alrm;
    sigemptyset(&alrm.sa_mask);
    alrm.sa_flags = SA_RESTART;
    if (sigaction(SIGALRM, &alrm, NULL) == -1)
        exit(0);
    int salida, cont = 0;

    // Se envía un SIGALRM cada segundo.
    alarm(1);
    // Se repite siempre hasta que haya que salir.
    while (1)
    {
        // Estará en pausa mientras no llegue una señal.
        pause();
        // Si la señal es SIGUSR1...
        if (flag_usr1 == 1)
        {
            // Se restablece la variable y se crea otro hijo.
            flag_usr1 = 0;
            printf("Señal SIGUSR1 recibida. Creando hijo.\n");
            salida = fork();
            cont += 1;
            // El hijo se detiene a sí mismo hasta que reciba SIGCONT del padre. Tras eso, termina.
            if (salida == 0)
            {
                raise(SIGSTOP);
                printf("SIGCONT recibido. Terminando.\n");
                exit(0);
            }
        }

        // Si la señal es SIGALRM...
        if (flag_alrm == 1)
        {
            // Se restablece la variable y se imprime el número de hijos.
            flag_alrm = 0;
            printf("Número de hijos creados: %d.\n", cont);
            // Si hay más de 5, les envía el SIGCONT,
            if (cont > 5)
            {
                printf("Hay más de 5 hijos. Terminando.\n");
                kill(0, SIGCONT);
                // y espera a que todos terminen.
                while (wait(NULL) > 0)
                {
                }
                printf("Acaba el padre.\n");
                exit(0);
            }
            // Se restablece la alarma.
            alarm(1);
        }
    }
}

static void gestion_usr1(int señal)
{
    flag_usr1 = 1;
}
static void gestion_alrm(int señal)
{
    flag_alrm = 1;
}
