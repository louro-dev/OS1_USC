#include "libs.h"

int main(int argc, char *argv[])
{
    // definimos variables que guardaran pos PIDs
    int hijo1 = 0, hijo2 = 0;

    // abrimos fork del hijo1
    if ((hijo1 = fork()) == 0)
    {
        printf("hijo 1 creado, esperando...\n");
        pause();
        exit(EXIT_SUCCESS);
    }
    else
    {
        // abrimos fork del hijo2
        if ((hijo2 = fork()) == 0)
        {
            printf("hijo 2 creado\n");
            // el hijo 2 mata al hijo1
            kill(hijo1, SIGINT);
            printf("se ha matado al hijo 1\n");
            exit(EXIT_SUCCESS);
        }
        else
        {
            // el padre espera a que acabe el hijo 1
            waitpid(hijo1, NULL, 0);

            exit(EXIT_SUCCESS);
        }
    }
}