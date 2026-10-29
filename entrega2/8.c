#include "libs.h"

void manejador_sigUsr1(int sig)
{
    printf("\nSe ha recivido la señal SIGUSR1\n");
}
void manejador_sigUsr2(int sig)
{
    printf("\nSe ha recivido la señal SIGUSR2\n");
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
        printf("Error al bloquear la señal SIGUSR1");
        exit(1);
    }

    struct sigaction usr1;
    usr1.sa_handler = manejador_sigUsr1;
    sigemptyset(&usr1.sa_mask);
    usr1.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &usr1, NULL) == -1)
    {
        printf("error al enlazar el manejador de SIGUSR1");
    }

    struct sigaction usr2;
    usr2.sa_handler = manejador_sigUsr2;
    sigemptyset(&usr2.sa_mask);
    usr2.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR2, &usr2, NULL) == -1)
    {
        printf("error al enlazar el manejador de SIGUSR2");
    }

    int H1 = 0, P = getpid();
    if ((H1 = fork()) == 0)
    {
        int N1 = 0;
        if ((N1 = fork()) == 0)
        {
            kill(P, SIGUSR2);
            printf("[N1]Esperando 5 segundos para terminar");
            sleep(5);
            printf("\n[N1]terminando con codigo 6\n");
            exit(6);
        }
        else
        {
            printf("\nterminando nieto1 con elo codigo 7\n");
            int statusN1, exitInf;

            // H1 le envia SIGUSR1 a P
            kill(P, SIGUSR1);

            // esperamos a que acabe el nieto de P (hijo de H1) y mostramos el resultado de su exit
            waitpid(N1, &statusN1, 0);
            if (WIFEXITED(statusN1))
            {
                exitInf = WIFEXITED(statusN1);
                printf("[H1] El nieto %d terminó normalmente. Su código de salida fue: %d\n", N1, exitInf);
            }
            printf("\n[H1]terminando con codigo 7\n");
            exit(7);
        }
    }
    else
    {
        int statusH1, exitInf;

        printf("[P]Señal SIGUSR1 bloqueada, esperando 5 segundos para que acabe N1");
        sleep(5);

        if (sigpending(&pendientes) == -1)
        {
            printf("[P] Error con el sigpending");
            return EXIT_FAILURE;
        }

        int pdng = sigismember(&pendientes, SIGUSR1);

        if (pdng == 1)
        {
            printf("[P] la señal SIGUSR1 sigue bloqueada");
        }
        if (pdng == 0)
        {
            printf("[P] error, la señal SIGUSR1 no esta pendiente");
        }
        else
        {
            printf("[P] Error, valor invalido de sigismember");
        }

        // desbloqueamos la señal
        if (sigprocmask(SIG_UNBLOCK, &block, NULL) == -1)
        {
            perror("Error al desbloquear");
            exit(EXIT_FAILURE);
        }

        waitpid(H1, &statusH1, 0);
        if (WIFEXITED(statusH1))
        {
            exitInf = WIFEXITED(statusH1);
            printf("[Padre] El hijo %d terminó normalmente. Su código de salida fue: %d\n", H1, exitInf);
        }
    }
}