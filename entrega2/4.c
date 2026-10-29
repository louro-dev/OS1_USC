#include "libs.h"

void manejador_sigint(int sig)
{
    static int cont = 0;
    cont++;
    if (cont < 3)
    {
        printf("\nseñal SIGINT atrapada y desactivada\n");
        fflush(stdout);
    }
    else
    {
        char answ;
        printf("\nCtrl+C ha sido pulsado 3 veces.\nDesea restaurar su funcionamiento original?\n(S/N):");
        fflush(stdout);

        scanf(" %c", &answ);
        switch (answ)
        {
        case 'S':
        case 's':
            signal(SIGINT, SIG_DFL);
            printf("Restaurando funcionalidad de Ctrl+C.\nEl proximo Ctrl+C cerrará el programa");
            break;

        case 'n':
        case 'N':
            printf("manteniendo el Ctrl+c desactivado\n");
            cont = 0;
            break;

        default:
            break;
        }
    }
}

int main()
{
    // asociamos la señal de SIGINT al manejador que hemos hecho
    if (signal(SIGINT, manejador_sigint) == SIG_ERR)
    {
        // si no se enlaza correctamente da error
        printf("error al asignar el manejador");
        exit(EXIT_FAILURE);
    }

    printf("Proceso iniciado, PID=%d\n", getpid());

    // empezamos bucle infinito del proceso
    while (1)
    {
        // pausa la ejecucion hasta recivir una señal(evita saturar la CPU)
        pause();
    }

    exit(EXIT_SUCCESS);
}