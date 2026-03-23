#include <stdio.h>
#include <stdlib.h>

/*
 * Lee un archivo
*/
int read_file(char *);

int main(int argc, char *argv[]) {
    if ( argc < 1 ) {
        printf("Uso: %s <archivo a abrir>\n", argv[0]);
        return 1;
    }

    if( !read_file(argv[1]) ) {
        return 1;
    }

}


// Lee un archivo a la salida estándar
int read_file(char * myfile) {
    FILE *fptr;
    char buffer[256];

    fptr = fopen(myfile, "r");
    if (fptr == NULL ) {
        printf("Error opening file: %s\n", myfile);
        return 0;
    }
    while(fgets(buffer, 100, fptr)) {
        printf("%s", buffer);
    }

    fclose(fptr);
}
