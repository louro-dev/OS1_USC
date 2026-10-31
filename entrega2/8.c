#include "libs.h"

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

void manejador_sigUsr1(int sig)
{
    printf("\n%s--[M] Se ha recibido la señal SIGUSR1\n", hora());
}
void manejador_sigUsr2(int sig)
{
    printf("%s--[M] Se ha recibido la señal SIGUSR2\n", hora());
}

int main()
{
    // creamos conjunto de señales que se bloquearan (solo se añade SIGUSR1)
    sigset_t block, pendientes;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);

    // bloqueamos SIGUSR1 antes de que H1 lo mande
    if ((sigprocmask(SIG_BLOCK, &block, NULL)) == -1)
    {
        printf("%s--Error al bloquear la señal SIGUSR1\n", hora());
        exit(1);
    }
    printf("%s--[P] Señal SIGUSR1 bloqueada\n", hora());

    struct sigaction usr1;
    usr1.sa_handler = manejador_sigUsr1;
    sigemptyset(&usr1.sa_mask);
    usr1.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &usr1, NULL) == -1)
    {
        printf("%s--[P] Error al enlazar el manejador de SIGUSR1\n", hora());
    }

    struct sigaction usr2;
    usr2.sa_handler = manejador_sigUsr2;
    sigemptyset(&usr2.sa_mask);
    usr2.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR2, &usr2, NULL) == -1)
    {
        printf("%s--[P] error al enlazar el manejador de SIGUSR2\n", hora());
    }

    int H1 = 0, P = getpid();
    if ((H1 = fork()) == 0)
    {
        printf("%s--[H1] He sido creado\n", hora());
        int N1 = 0;
        if ((N1 = fork()) == 0)
        {
            //--N1
            printf("%s--[N1] He sido creado\n", hora());
            kill(P, SIGUSR2);
            printf("%s--[N1] Enviando señal SIGUSR2\n", hora());
            printf("%s--[N1] Durmiendo...\n", hora());
            sleep(5);
            printf("\n%s--[N1] Terminado\n", hora());
            exit(6);
        }
        else
        {
            //--H1
            int statusN1, exitInf;

            // H1 le envia SIGUSR1 a P
            kill(P, SIGUSR1);
            printf("%s--[H1] Enviando señal SIGUSR1\n", hora());

            // esperamos a que acabe el nieto de P (hijo de H1) y mostramos el resultado de su exit
            waitpid(N1, &statusN1, 0);
            if (WIFEXITED(statusN1))
            {
                exitInf = WIFEXITED(statusN1);
                printf("%s--[H1] El nieto con PID %d terminó normalmente. Su código de salida fue: %d\n", hora(), N1, exitInf);
            }
            printf("%s--[H1] Terminado\n", hora());
            exit(7);
        }
    }

    //--PADRE
    int statusH1, exitInf;

    printf("%s--[P] Esperando 5 segundos para que acabe N1\n", hora());
    sleep(5);

    if (sigpending(&pendientes) == -1)
    {
        printf("%s--[P] Error con el sigpending\n", hora());
        return EXIT_FAILURE;
    }

    int pdng = sigismember(&pendientes, SIGUSR1);

    if (pdng == 1)
    {
        printf("%s--[P] La señal SIGUSR1 sigue bloqueada\n", hora());
    }
    else if (pdng == 0)
    {
        printf("%s--[P] Error, la señal SIGUSR1 no esta pendiente\n", hora());
    }
    else
    {
        printf("%s--[P] Error, valor invalido de sigismember\n", hora());
    }

    printf("%s--[P] SIGUSR1 desbloqueada", hora());
    // desbloqueamos la señal
    if (sigprocmask(SIG_UNBLOCK, &block, NULL) == -1)
    {
        printf("%s--[P] Error al desbloquear\n", hora());
        exit(EXIT_FAILURE);
    }

    printf("%s--[P] Esperando a que termine H1\n", hora());
    waitpid(H1, &statusH1, 0);
    if (WIFEXITED(statusH1))
    {
        exitInf = WIFEXITED(statusH1);
        printf("%s--[P] El hijo con PID %d terminó normalmente. Su código de salida fue: %d\n", hora(), H1, exitInf);
    }
}