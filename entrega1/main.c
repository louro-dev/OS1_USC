#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{

    int opt, n_val = 0, p_val = 0, n_flag = 0, p_flag = 0;

    // Procesar las opciones -n y -p
    while ((opt = getopt(argc, argv, "n:p:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            n_val = atoi(optarg); // Convertir string a entero
            n_flag = 1;
            break;
        case 'p':
            p_val = atoi(optarg);
            p_flag = 1;
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    // Verificar que se hayan pasado ambos argumentos
    if (!n_flag || !p_flag)
    {
        printf("Error: Se requieren ambas opciones -n y -p.\n");
        exit(EXIT_FAILURE);
    }

    // abrimos documento en escritura
    FILE *fp = fopen("output.txt", "w+");

    // calculamos el tamaño de cada chunck que calculara cada hijo y el resto
    int chunck = n_val / p_val, remainder = n_val % p_val;
    double res = 0;

    for (int i = 1; i <= p_val; i++)
    {
        // hacemos el fork para crear los procesos hijos
        if (fork() == 0)
        {
            // si es el primero entra por otro camino
            if (i == 1)
            {
                // calulamos el inicio y el final del bucle para el primer hijo
                int start = 1, end = chunck + remainder;
                // iniciamos el reloj
                clock_t start_child = clock();
                // bucle de operaciones
                for (int j = start; j <= end; j++)
                {
                    res += tan(sqrt(j));
                }
                // paramos reloj y calculamos el tiempo
                clock_t end_child = clock();
                double time_spent_child = ((double)(end_child - start_child)) / CLOCKS_PER_SEC;
                // imprimimos resultados en terminal y documento auxiliar
                printf("Child: %d time: %f result: %lf\n", getpid(), time_spent_child, res);
                fprintf(fp, "Child: %d time: %f result: %lf\n", getpid(), time_spent_child, res);
                exit(EXIT_SUCCESS);
            }
            else
            {
                // calulamos el inicio y el final del bucle para cada proceso hijo
                int start = (i - 1) * chunck + remainder + 1, end = i * chunck + remainder;

                // iniciamos el reloj
                clock_t start_child = clock();
                // bucle de operaciones
                for (int j = start; j <= end; j++)
                {
                    res += tan(sqrt(j));
                }
                // paramos reloj y calculamos el tiempo
                clock_t end_child = clock();
                double time_spent_child = ((double)(end_child - start_child)) / CLOCKS_PER_SEC;
                // imprimimos resultados en terminal y documento auxiliar
                printf("Child: %d time: %f result: %lf\n", getpid(), time_spent_child, res);
                fprintf(fp, "Child: %d time: %f result: %lf\n", getpid(), time_spent_child, res);
                exit(EXIT_SUCCESS);
            }
        }
    }
    fclose(fp);

    // esperamos a que acaben todos los hijos
    for (int i = 0; i < p_val; i++)
    {
        wait(NULL); // Esperar a que los procesos hijos termine
    }

    // hijo P+1
    double promedio = 0;
    if (fork() == 0)
    {
        FILE *archivo = fopen("output.txt", "r");
        if (archivo == NULL)
        {
            perror("Error al abrir el archivo");
            return 1;
        }

        char linea[1024];
        double resultados = 0;
        int cantidad = 0;
        double valor_temporal;

        // Leer línea por línea
        while (fgets(linea, sizeof(linea), archivo) != NULL)
        {
            // %*d y %*f leen el entero y el flotante pero los descartan.
            // Solo guardamos el %lf en valor_temporal.
            if (sscanf(linea, "Child: %*d time: %*f result: %lf", &valor_temporal) == 1)
            {
                // Almacenar el valor
                resultados += valor_temporal;
                cantidad++;
            }
        }
        fclose(archivo);
        promedio = resultados / n_val;
        printf("Promedio de resultados: %lf\n", promedio);
        FILE *fp = fopen("output.txt", "a");
        if (fp == NULL)
        {
            perror("Error al abrir el archivo para escribir el promedio");
            return 1;
        }
        else
        {
            fprintf(fp, "Promedio de resultados: %lf\n", promedio);
        }
        fclose(fp);
        exit(EXIT_SUCCESS);
    }
    wait(NULL); // Esperar a que el proceso hijo P+1 termine

    // hijo P+2
    double media = 0;
    if (fork() == 0)
    {
        double result = 0;
        for (int i = 1; i <= n_val; i++)
        {
            result += tan(sqrt(i));
        }
        media = result / n_val;
        printf("Resultado del proceso P+2: %lf\n", media);
        FILE *fp = fopen("output.txt", "a");
        if (fp == NULL)
        {
            perror("Error al abrir el archivo para escribir el resultado del proceso P+2");
            return 1;
        }
        else
        {

            fprintf(fp, "Resultado del proceso P+2: %lf\n", media);
        }
        fclose(fp);
        exit(EXIT_SUCCESS);
    }
    wait(NULL); // Esperar a que el proceso hijo P+2 termine

    // calculamos la diferencia
    double diferencia = fabs(media - promedio);
    printf("Diferencia entre el promedio y el resultado del proceso P+2: %lf\n", diferencia);
    FILE *fpf = fopen("output.txt", "a");
    if (fpf == NULL)
    {
        perror("Error al abrir el archivo para escribir la diferencia");
        return 1;
    }
    else
    {
        fprintf(fpf, "Diferencia entre el promedio y el resultado del proceso P+2: %lf\n", diferencia);
    }
    fclose(fpf);

    return EXIT_SUCCESS;
}
