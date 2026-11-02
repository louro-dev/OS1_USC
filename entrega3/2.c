#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>



//FALTA CALCULAR EL TIEMPO DE EJECUCION DE CADA HILO Y EL TIEMPO TOTAL DE EJECUCION DEL PROGRAMA
//PARA CALCULAR EL SPEEDUP



//ejemplo de ejecucion: ./2 -h <nHilos> -i <nIteraciones>
#define DOC "res.txt"

double operacion(int nInicio,int nPasos)
{
    double resultado=0;
    for(int i=0;i<nPasos;i++)
    {
        resultado+=(double)(1.0/pow(nInicio+i,2));
    }
    return resultado;
}

void *th_0(void *arg)
{
    //abrimos documento en modo append para que no se sobreescriba el resultado de los hilos
    FILE *f = fopen(DOC, "a");
    //recivimos el numero de inicio y el numero de pasos que debe hacer cada hilo
    int nInicio = *((int *)arg);
    int nPasos = *((int *)(arg + sizeof(int)));
    //calculamos el resultado de la operacion y lo imprimimos en pantalla y en el documento
    double resultado = operacion(nInicio, nPasos);
    printf("[Hilo %d] Resultado: %f\n", nInicio, resultado);
    fprintf(f, "Hilo %d;Resultado %f\n", nInicio, resultado);
    //cerramos el documento y salimos del hilo
    fclose(f);
    pthread_exit((void *)1);
}

int main(int argc, char *argv[])
{
  int nHilos = 0,nIteraciones = 0,opcion,prop=0;

  //se utiliza getopt para parsear los argumentos de la linea de comandos
  while ((opcion = getopt(argc, argv, "h:i:")) != -1)
  {
    char *fin;
    errno = 0;
    long valor = strtol(optarg, &fin, 10);

    if (errno == ERANGE || *fin != '\0' || valor <= 0 || valor > INT_MAX)
    {
      fprintf(stderr, "El argumento de -%c debe ser un entero positivo\n", opcion);
      return EXIT_FAILURE;
    }
    switch (opcion)
    {
      case 'h':
        nHilos = (int) valor;
        break;
      case 'i':
        nIteraciones = (int) valor;
        break;
      default:
        fprintf(stderr, "Uso: %s -h <nHilos> -i <nIteraciones>\n", argv[0]);
        return EXIT_FAILURE;
    }
  }

  //imprimimos numero de hilos e iteraciones
  printf("Número de hilos: %d\n", nHilos);
  printf("Número de iteraciones: %d\n", nIteraciones);

  //comprobamos si el numero de iteraciones es multiplo del numero de hilos, si no lo es, imprimimos un mensaje indicando que los primeros 
  //hilos harán una iteración más que el resto
  int comp=nIteraciones%nHilos,prop = nIteraciones / nHilos;
  if(comp!=0)
  {
    printf("El número de iteraciones no es multiplo del numero de hilos\n");
    printf("los primeros %d hilos harán %d iteraciones y el resto de hilos harán %d iteraciones\n", prop+1, prop+comp, prop);
  }
  else
  {
    printf("Cada hilo hará %d iteraciones\n", prop);
  }


  pthread_t hilo[nHilos];
  
  for(int i=0;i<nHilos;i++)
  {
    if(comp==0){
        //guardamos memoria para los argumentos
    int *inf = malloc(2 * sizeof(int));

        //asignamos el numero de inicio y el numero de pasos que debe hacer cada hilo
    inf[0]=i*prop;
    inf[1]=prop;

        //creamos el hilo y le pasamos los argumentos, y lo cerramos despues de hacer sus operaciones
    pthread_create(&hilo[i], NULL, th_0, inf);
    pthread_exit((void *)1);

    }
    else{
        //guardamos memoria para los argumentos
    int *inf = malloc(2 * sizeof(int));

        //asignamos el numero de inicio y el numero de pasos que debe hacer cada hilo
    inf[0]=i*prop;
    inf[1]=prop;

        //si el numero de hilo es menor que el numero de cuentas que quedan por hacer, se le suma una iteración más al número de pasos que debe hacer
    if(i<comp){
        inf[1]++;
    }

        //creamos el hilo y le pasamos los argumentos, y lo cerramos despues de hacer sus operaciones
    pthread_create(&hilo[i], NULL, th_0, inf);
    }
  }

  for(int i=0;i<nHilos;i++)
  {
    pthread_join(hilo[i], NULL);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //leemos del documento y realizamos la suma final de los resultados de los hilos, y la imprimimos en pantalla

  FILE *f = fopen(DOC, "666");
  double resultadoFinal=0;
  int hilo;
  double resultado;
  while(fgets(f, 100, f) != NULL)
  {
    fscanf(f, "Hilo %d;Resultado %lf\n", &hilo, &resultado);
    resultadoFinal+=resultado;
  }
  printf("Resultado final: %f\n", resultadoFinal);
  fprintf(f, "Resultado final %f\n", resultadoFinal);
  fclose(f);

  /////////////////////////////////////////////////////////////
  //Hacemos el calculo secuencial de la operacion para comparar

  for(int i=0;i<nIteraciones;i++)
  {
    resultadoFinal+=1.0/pow(i,2);
  }
  printf("Resultado secuencial: %f\n", resultadoFinal);


  return EXIT_SUCCESS;
}