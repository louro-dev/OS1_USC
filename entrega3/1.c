

/*
#define NHILOS 2
void *th_0(void *arg)
{
    sleep(5);
    printf("[Hilo %d] Soy un hilo\n", *((int *)arg));
    pthread_exit((void *)1);
}

void *th_1(void *arg)
{
    sleep(2);
    printf("[Hilo %d] Soy un hilo\n", *((int *)arg));
    pthread_exit((void *)1);
}

int main(void)
{

    pthread_t hilos[NHILOS];
    int ids[NHILOS];

    ids[0] = 0;
    ids[1] = 1;
    pthread_create(&hilos[0], NULL, th_0, &ids[0]);
    pthread_create(&hilos[1], NULL, th_1, &ids[1]);

    pthread_join(hilos[1], NULL);
    pthread_join(hilos[0], NULL);


    return 0;
}
    Hacemos una funcion para cada hilo, permitiendo que cada hilo tenda un tiempo de sleep diferente, haciendo que
    el hilo 1 termine antes que el hilo 0, a pesar de que el hilo 0 fue creado primero.*/


/*    
#define NHILOS 3

void *th_0(void *arg)
{
    printf("[Hilo %d] Soy un hilo\n", *((int *)arg));
    pthread_exit((void *)1);
    sleep(10);
}

void *th_1(void *arg)
{
    printf("[Hilo %d] Soy un hilo\n", *((int *)arg));
    int st=fork();
    if(st==0)
    {
        char buff[10];
        printf("[Hijo %d] Soy un hijo, mi padre es %d\nesperare a que escribas algo para que puedas verme con ps -ef:"
            , getpid(), getppid());
        scanf(" %[^\n]", buff);
        printf("[Hijo %d] Has escrito %s\n", getpid(), buff);
    }
    else{
        wait(NULL);
        printf("[Padre %d] Soy el padre, mi hijo es %d\n", getpid(), st);
    }

    pthread_exit((void *)1);
}

int main(void)
{
    pthread_t hilos[NHILOS];
    int ids[NHILOS];

    for (int i = 0; i < NHILOS-1; i++)
    {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, th_0, &ids[i]);
    }
    ids[NHILOS-1] = NHILOS-1;
    pthread_create(&hilos[NHILOS-1], NULL, th_1, &ids[NHILOS-1]);

    for (int i = 0; i < NHILOS; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    exit(0);

}   
    */

    /*
#define NHILOS 3

void *th_0(void *arg)
{
    //los dormimos antes de que impriman nada para ver que pasa si se hace el exit sin que hayan acabado de ejecutarse los hilos
    sleep(5);
    printf("[Hilo %d] Soy un hilo\n", *((int *)arg));
    pthread_exit((void *)1);
}

void *th_1(void *arg)
{
    printf("[Hilo %d] Soy un hilo\n", *((int *)arg));

    exit(0);
}

int main(void)
{
    pthread_t hilos[NHILOS];
    int ids[NHILOS];

    for (int i = 0; i < NHILOS-1; i++)
    {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, th_0, &ids[i]);
    }
    ids[NHILOS-1] = NHILOS-1;
    pthread_create(&hilos[NHILOS-1], NULL, th_1, &ids[NHILOS-1]);

    for (int i = 0; i < NHILOS; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    exit(0);
}   
    //si se hace exit en un hilo, el proceso termina y los otros hilos no acaban de ejecutarse, por lo que no se imprime nada de lo que hacen los
    //otros hilos.
    */