#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global = 100;

int main() {
    int local = 0;
    int *dinamica = (int*)malloc(sizeof(int));
    *dinamica = 50;
    FILE *archivo = fopen("Prueba.txt", "r");
    if(archivo == NULL) {
        printf("No se ha podido abrir el archivo.\n");
        exit(1);
    }

    printf("Empezamos el programa!\n");
    printf("\n");

    int r = fork();

    if(r < 0) {
        printf("Error, no se ha podido crear el hijo\n");
        free(dinamica);
        fclose(archivo);
        exit(1);
    }
    else if(r == 0) {
        printf("\n\nSoy el proceso hijo (PID: %d), con padre (PID: %d)\n", (int) getpid(), (int) getppid());
        printf("Hijo --> UID real (quien lo ejecuta): %d, UID efectivo (permisos activos): %d, GID (grupo principal del usuario): %d\n", (int) getuid(), (int) geteuid(), (int) getgid());
        printf("Hijo --> Variable de entorno USER: %s\n", getenv("USER"));

        local+=1;
        global+=1;
        *dinamica+=1;
        printf("\nDirecciones y valores en el HIJO: \n");
        printf("Local: local = %d en posicion %p\n", local, &local);
        printf("Dinamica: dinamica = %d en posicion %p\n", *dinamica, dinamica);
        printf("Global: global = %d en posicion %p\n", global, &global);

        char linea[256];
        if (fgets(linea, sizeof(linea), archivo) != NULL) {
            printf("Hijo --> Lectura del archivo: %s", linea);
        } else {
            printf("Hijo --> El archivo está vacío o no se pudo leer nada.\n");
        }        
    }
    else {
        printf("\n\nSoy el proceso padre (PID: %d), con padre (PID: %d)\n", (int) getpid(), (int) getppid());
        printf("Padre --> UID real (quien lo ejecuta): %d, UID efectivo (permisos activos): %d, GID (grupo principal del usuario): %d\n", (int) getuid(), (int) geteuid(), (int) getgid());
        printf("Padre --> Variable de entorno USER: %s\n", getenv("USER"));

        printf("\nDirecciones y valores en el PADRE: \n");
        printf("Local: local = %d en posicion %p\n", local, &local);
        printf("Dinamica: dinamica = %d en posicion %p\n", *dinamica, dinamica);
        printf("Global: global = %d en posicion %p\n", global, &global);
    }
    printf("Escribe un numero: ");
    scanf("%d", &local);
    printf("Soy el proceso %d con numero x = %d\n", (int) getpid(), local);
    printf("\n");

    free(dinamica);
    fclose(archivo);

    exit(0);
}