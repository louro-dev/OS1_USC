#include "libs.h"

void manejador_sigint(int sig)
{
    // restauramls funcionalidad del ctrl+C
    signal(SIGINT, SIG_DFL);
    printf("Restaurando funcionalidad de Ctrl+C.\nEl proximo Ctrl+C cerrará el programa");
}

int main()
{
    // declaramos struct de sigaction
    struct sigaction sgnl;
    sgnl.sa_handler = manejador_sigint;
    sigemptyset(&sgnl.sa_mask);
    sgnl.sa_flags = SA_RESTART; // reinicia las llamadas al sistema interrumpidas

    // llamamos a sigaction con la señal SIGINT (ctrl+c), &sgnl (la struct que tiene la funcion manejador, y las flags), y NULL
    // porque no hay configuracion antigua
    if (sigaction(SIGINT, &sgnl, NULL) == -1)
    {
        // si no se enlaza correctamente da error
        printf("error al asignar el manejador");
        exit(EXIT_FAILURE);
    }

    // empezamos bucle infinito del proceso
    while (1)
    {
        // pausa la ejecucion hasta recivir una señal(evita saturar la CPU)
        pause();
    }

    exit(EXIT_SUCCESS);
}