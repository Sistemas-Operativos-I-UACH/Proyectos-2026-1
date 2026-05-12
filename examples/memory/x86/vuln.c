#include <stdio.h>
#include <string.h>

void vulnerable_function(char *str) {
    char buffer[64];
    // strcpy no verifica la longitud, introduciendo el desbordamiento
    strcpy(buffer, str);
    printf("Datos copiados exitosamente.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <texto>\n", argv[0]);
        return 1;
    }
    vulnerable_function(argv[1]);
    return 0;
}
