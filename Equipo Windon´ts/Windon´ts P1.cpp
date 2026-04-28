#include <stdio.h>

/*a355214 Gibrán Zurid Abbud González
a376899 Ana Ruvi Morales Chavez
a367978 Daniel Ramses Reyes Benitez*/

int main(int argc, char *argv[])
{
    FILE *archivo;
    char caracter;

    // Verificar que se haya pasado el nombre del archivo
    if (argc != 2)
    {
        printf("Uso: %s nombre_archivo\n", argv[0]);
        return 1;
    }

    // Abrir el archivo en modo lectura
    archivo = fopen(argv[1], "r");

    if (archivo == NULL)
    {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    // Leer y mostrar el contenido
    while ((caracter = fgetc(archivo)) != EOF)
    {
        printf("%c", caracter);
    }

    fclose(archivo);

    return 0;
}