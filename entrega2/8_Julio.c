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
#include <time.h>
#include <sys/time.h>

static void gestion_usr1(int);
static void gestion_usr2(int);
char *hora();

// Punto de inicio.
int main(void)
{
    int salida, status;

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

    // El mismo bloque de código, para SIGUSR2.
    struct sigaction usr2;
    usr2.sa_handler = gestion_usr2;
    sigemptyset(&usr2.sa_mask);
    usr2.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR2, &usr2, NULL) == -1)
        exit(0);

    // Declaramos las variables de dos conjuntos de señales.
    sigset_t conjunto, pendientes;
    // Se inicializa el conjunto vacío.
    sigemptyset(&conjunto);
    // Se añade la señal SIGUSR1 al conjunto.
    sigaddset(&conjunto, SIGUSR1);
    // Bloquea las señales del conjunto añadiéndolas a las que el proceso ya tenía bloqueadas.
    sigprocmask(SIG_BLOCK, &conjunto, NULL);
    printf("P  %s: Señal SIGUSR1 bloqueada.\n", hora());

    salida = fork();
    if (salida > 0)
        printf("P  %s: H1 creado.\n", hora());
    if (salida == 0)
    {
        // Se envía la señal SIGUSR1 al padre.
        kill(getppid(), SIGUSR1);
        printf("H1 %s: Señal SIGUSR1 enviada a P.\n", hora());

        int P = getppid(), nieto = fork();
        if (nieto > 0)
            printf("H1 %s: N1 creado.\n", hora());
        if (nieto == 0)
        {
            // Se envía la señal SIGUSR2 al abuelo.
            printf("N1 %s: Enviando señal SIGUSR2 a P.\n", hora());
            kill(P, SIGUSR2);
            printf("N1 %s: Durmiendo...\n", hora());
            sleep(5);
            printf("N1 %s: Terminando.\n", hora());
            exit(2);
        }

        printf("H1 %s: Esperando a que acabe N1.\n", hora());
        wait(&status);
        status = WEXITSTATUS(status);
        printf("H1 %s: Salida de N1: %d\n", hora(), status);
        printf("H1 %s: Terminando.\n", hora());
        exit(1);
    }

    printf("P  %s: Durmiendo 2 segundos...\n", hora());
    // El sleep() se interrumpe al llegar una señal. Con este bucle se fuerza a que duerma el tiempo que le falte.
    unsigned int dormir = sleep(2);
    while (dormir != 0)
    {
        dormir = sleep(dormir);
    }
    // Se añaden las señales pendientes a ese conjunto.
    sigpending(&pendientes);
    // "señal" se pone a 1 si SIGUSR1 está en el conjunto de señales pendientes.
    int señal = sigismember(&pendientes, SIGUSR1);
    // Si lo está, se imprime el mensaje.
    if (señal == 1)
    {
        printf("P  %s: La señal SIGUSR1 está bloqueada.\n", hora());
    }
    else
    {
        printf("P  %s: No se pudo bloquear SIGUSR1.\n", hora());
    }

    // Ahora se desbloquea la señal.
    sigprocmask(SIG_UNBLOCK, &conjunto, NULL);
    printf("P  %s: Señal SIGUSR1 desbloqueada.\n", hora());
    printf("P  %s: Esperando a que termine H1.\n", hora());
    wait(&status);
    status = WEXITSTATUS(status);
    printf("P  %s: Salida de H1: %d\n", hora(), status);
    printf("P  %s: Terminando.\n", hora());
    exit(0);
}

static void gestion_usr1(int señal)
{
    printf("P  %s: Señal SIGUSR1 recibida.\n", hora());
}
static void gestion_usr2(int señal)
{
    printf("P  %s: Señal SIGUSR2 recibida.\n", hora());
}

char *hora(void)
{
    // Buffer estático: sigue existiendo cuando la función termina, así que se puede devolver.
    // "HH:MM:SS.uuuuuu" son 15 caracteres, más el '\0'.
    static char hora[28];
    // La función gettimeofday() guarda los segundos desde el 1 de enero de 1970 (tv_sec)
    //      y los microsegundos transcurridos dentro de ese segundo (tv_usec).
    struct timeval tiempo;
    gettimeofday(&tiempo, NULL);
    // Convierte los segundos en fecha y hora locales, separadas en campos (hora, minuto...).
    struct tm local;
    localtime_r(&tiempo.tv_sec, &local);
    // Escribe solo horas, minutos y segundos: "22:39:32" (8 caracteres).
    strftime(hora, sizeof hora, "%H:%M:%S", &local);
    // A partir de la posición 8 añade un punto y los microsegundos con 6 cifras: ".417305".
    snprintf(hora + 8, sizeof hora - 8, ".%06ld", (long)tiempo.tv_usec);
    return hora;
}
