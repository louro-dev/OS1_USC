#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define RUN_TIME_SEC 10

volatile sig_atomic_t keep_running = 1;

void timer_handler(int signum)
{
    keep_running = 0;
}

int main()
{
    FILE *fp = fopen("/dev/null", "w");
    if (fp == NULL)
    {
        perror("Error abriendo archivo");
        return 1;
    }

    // TRUCO 1: Asignar un buffer gigante (1 MB)
    char my_io_buffer[1024 * 1024];
    if (setvbuf(fp, my_io_buffer, _IOFBF, sizeof(my_io_buffer)) != 0)
    {
        perror("Error configurando el buffer");
        return 1;
    };
    long long operaciones = 0;

    signal(SIGALRM, timer_handler);
    alarm(RUN_TIME_SEC);

    printf("Iniciando %d segundos de I/O ultrarrápido...\n", RUN_TIME_SEC);

    // --- BUCLE CALIENTE (HOT LOOP) ---
    while (keep_running)
    {
        // TRUCO 3: putc_unlocked omite los bloqueos de concurrencia (mutex) del sistema.
        // Es inmensamente más rápido que fwrite o fputc normal para operaciones de 1 byte.
        putc_unlocked('a', fp);
        operaciones++;
    }
    // ---------------------------------

    fclose(fp);

    double ops_per_sec = (double)operaciones / RUN_TIME_SEC;
    double mb_per_sec = (ops_per_sec * 1) / (1024.0 * 1024.0);

    printf("\n\n--- RESULTADOS MÁXIMOS ---\n\n");
    printf("Operaciones totales: %lld\n", operaciones);
    printf("Operaciones por segundo: %.2f\n", ops_per_sec);
    printf("Rendimiento equivalente: %.2f MB/s\n", mb_per_sec);

    return 0;
}