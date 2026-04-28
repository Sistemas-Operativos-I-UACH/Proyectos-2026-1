#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *archivo; //Apuntador de archivo
    char c; //Variable tipo caracter

//Solo verifica que el usuario escriba correctamente el nombre del archivo
    if(argc < 2)
    {
        printf("Hay un error: Debe escribir el nombre del archivo en la linea de comando, intenta de nuevo\n");
        printf("Ejemplo: ./programa datos.txt\n");
        return 1;
    }
	//Solo abre el archivo en modo lectura
    archivo = fopen(argv[1], "r");
	
	// Verifica si el archivo se abrió correctamente
    if(archivo == NULL)
    {
        printf("No se pudo abrir el archivo\n");
        return 1; //si hubo error termina el programa
    }

    printf("Contenido del archivo:\n\n");
    
	// lee el archivo caracter por caracter
    while((c = fgetc(archivo)) != EOF)
    {
        printf("%c", c); //imprime cada caracter leído
    }

    //cierra el archivo para liberar recursos
    fclose(archivo);
	return 0;
}
