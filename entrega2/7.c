#include "libs.h"

// variables globales flag
volatile sig_atomic_t flag_usr1 = 0;
volatile sig_atomic_t flag_alarm = 0;

// manejadores que activan las flags si se recive la señal
void manejador_usr1(int sig)
{
    flag_usr1 = 1;
}
void manejador_alarm(int sig)
{
    flag_alarm = 1;
}

int main(int argc, char *argv[])
{
    // variable que guarda la cantidad de hijos a lo largo ed la ejecucion
    int cantidadHijos = 0;

    // enlazamos SIGUSR1 con el manejador
    struct sigaction sgnl;
    sgnl.sa_handler = manejador_usr1;
    sigemptyset(&sgnl.sa_mask);
    sgnl.sa_flags = 0;
    if ((sigaction(SIGUSR1, &sgnl, NULL)) == -1)
    {
        printf("error en la llamada a sigaction");
        exit(EXIT_FAILURE);
    }

    // enlazamos alarm con el manejador
    struct sigaction alarm_action;
    alarm_action.sa_handler = manejador_alarm;
    sigemptyset(&alarm_action.sa_mask);
    alarm_action.sa_flags = 0;
    if ((sigaction(SIGALRM, &alarm_action, NULL)) == -1)
    {
        printf("error en la llamada a sigaction");
        exit(EXIT_FAILURE);
    }

    // imprimimos el pid del padre para verlo por pantalla y saber que se ha llegado aqui
    printf("Padre iniciado con PID %d\nEsperando señales...\n", getpid());

    // hace que sale la alarma en 1 segund
    alarm(1);

    // bucle de logica
    while (1)
    {
        // el padre espera hasta que reciva una señal
        pause();

        // si se ha recivido la señal SIGUSR1 se entra en el if, si no se reinicia el bucle y se queda de nuevo en el pause
        if (flag_usr1 == 1)
        {
            // reiniciamos la flag
            flag_usr1 = 0;

            // hacemos el fork para crear el proceso hijo
            int pid = 0;
            if ((pid = fork()) == -1)
            {
                // si da errorlo decimos y cortamos ejecucion
                printf("\nError con el fork\n");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                // si se crea correctamente avanzamos
                //-----codigo del hijo-----
                printf("\nHijo creado con PID %d\nentrando en espera...\n", getpid());
                raise(SIGSTOP);
                exit(0);
            }
            else
            {
                //-----codigo del padre-----

                // aumentamos la variable que guarda el numero de hijos y comprobamos que no se haya llegado a los 5
                cantidadHijos++;

                // comprobamos que la cantidad de hijos sea menor que 5
                if (cantidadHijos > 5)
                {
                    printf("\nMaximo de hijos (5) alcanzado, cerrando ejecucion...\n");
                    // salimos del bucle para que no se quede otra vez en el pause
                    break;
                }
            }
        }

        // comprobamos si ha saltado la alarma
        if (flag_alarm)
        {
            // reiniciamos el flag y imprimimos la informacion por terminal
            flag_alarm = 0;
            printf("[Tick] Procesos creados hasta ahora: %d\n", cantidadHijos);
            // Reprogramamos la alarma para el siguiente segundo
            alarm(1);
        }
    }

    //-----gestion de cerrado de todso los procesos-----

    // reactivamos los hijos para que recivan el sigterm
    kill(0, SIGCONT);

    // el padre espera por todos los hijos
    while (wait(NULL) > 0)
    {
    }

    // imprimimos por terminal mensaje de exito y cortamos tambien el programa padre
    printf("Todos los hijos han sido eliminador con exito\nterminando ejecucion...\n");
    exit(EXIT_SUCCESS);
}