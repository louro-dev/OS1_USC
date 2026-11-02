#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("Empezamos el programa!\n\n");
    int h1 = fork(); // Hijo 1
    
    if(h1 < 0) {
        printf("Error creando el hijo 1.\n");
        exit(1);
    }
    else if(h1 > 0) {
        int h2 = fork(); // Hijo 2

        if(h2 < 0) {
            printf("Error creando el hijo 2.\n");
            exit(1);
        }
        else if(h2 > 0) {
            printf("Soy el padre con PID = %d\n", (int) getpid());
            printf("Padre --> Durmiendo 10s. El hijo 1 estará en estado ZOMBIE.\n");
            sleep(10); // Duermo al padre para que el Hijo 1 se convierta en zombie.
            int status; // Variable donde el kernel almacenará la información codificada de estado del hijo
            pid_t pid_status = wait(&status); // Detiene al padre hasta que finalice un hijo (hijo 1) y devuelve su PID
            if(pid_status > 0 && WIFEXITED(status)) { // Comprueba si un hijo terminó y si finalizó correctamente mediante exit()
                printf("Padre --> El hijo con PID %d salio del wait().\n", (int) pid_status);
                printf("Padre --> Codigo de exit recogido del Hijo 1 (WEXITSTATUS): %d\n", WEXITSTATUS(status));
            }
            exit(0);
        }
        else {
            printf("Soy el hijo 2 con PID = %d\n", (int) getpid());
            printf("Hijo 2 --> Durmiendo 20s. Quedaré huérfano.\n");
            sleep(20); // Duermo al Hijo 2 para que quede huérfano.
            printf("\nHijo 2 con PID = %d. He despertado. Mi nuevo PPID es %d (adoptado).\n", (int) getpid(), (int) getppid());

            // Cambia la imagen del proceso por la de 'ls' (borra de memoria el programa C y carga 'ls')
            execlp("ls", "ls", NULL);

            // Este printf NUNCA se ejecutará salvo que execlp() falle y devuelva un error
            printf("Hijo 2 --> ERROR: Si ves este texto en consola, execlp() ha fallado.\n");
            exit(1);
        }
    }
    else {
        printf("Soy el hijo 1 con PID = %d\n", (int) getpid());
        exit(42); // El Hijo 1 lo finalizo para convertirlo en zombie.
    }
}