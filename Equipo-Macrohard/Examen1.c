#include <stdio.h>
#include <stdlib.h>

static void mostrar_uso(const char *programa) {
    fprintf(stderr, "Uso: %s <archivo>\n", programa);
}

static int mostrar_archivo(const char *ruta) {
    FILE *archivo = fopen(ruta, "r");
    char linea[512];

    if (archivo == NULL) {
        perror(ruta);
        return 0;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        fputs(linea, stdout);
    }

    fclose(archivo);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        mostrar_uso(argv[0]);
        return EXIT_FAILURE;
    }

    if (!mostrar_archivo(argv[1])) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}