#include "libs.h"

static void gestion(int numero_de_senhal)
{ // Funcion de gestion de señales
    switch (numero_de_senhal)
    {
    case SIGUSR1: // Entra señal SIGUSR1
        printf("Señal tipo 1 recibida.Soy %d\n\n", getpid());
        break;
    case SIGUSR2:
        printf("Señal de tipo 2 recibida. Soy %d\n\n", getpid());
        break;
    }
    return;
}

int main()
{
    pid_t padre, hijo;

    if (signal(SIGUSR1, gestion) == SIG_ERR)
        printf("Error al crear gestor 1\n");
    if (signal(SIGUSR2, gestion) == SIG_ERR)
        printf("Error al crear gestor 2\n");

    padre = getpid();

    if ((hijo = fork()) == 0)
    {
        // Trabajo del hijo
        kill(padre, SIGUSR1); // Envia señal al padre
        for (;;)
            ; // Espera señal del padre indefinidamente
    }
    else
    { // Trabajo del padre
        // sleep(1)
        kill(hijo, SIGUSR2);

        // sleep(1);

        kill(hijo, SIGTERM);

        // sleep(1);

        wait(NULL);
    }
    return EXIT_SUCCESS;
}

/*
sin el sleep(1), no existe el tiempo suficiente entre señales y el SIGTERM corta la ejecucion antes de que pueda imprimirse el
mensaje que correcponde con el envio de SIGUSR2 o SIGUSR1
*/