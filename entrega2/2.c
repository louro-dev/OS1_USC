#include "libs.h"

int main(int argc, char *argv[])
{
    char buffer[1000];

    printf("dea una frase: \n");
    scanf("%s", buffer);

    printf("%s", buffer);
    return EXIT_FAILURE;
}