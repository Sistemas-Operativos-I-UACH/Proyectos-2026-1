#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// Función recursiva para recorrer directorios
void listar_archivos(const char *ruta_base) {
    struct dirent *entrada;
    
    // 1. Intentamos abrir el directorio actual
    DIR *dir = opendir(ruta_base);

    // Si no tenemos permisos o el directorio no existe, salimos silenciosamente de esta rama
    if (dir == NULL) {
        return;
    }

    // 2. Leemos el contenido del directorio elemento por elemento
    while ((entrada = readdir(dir)) != NULL) {
        
        // 3. REGLA DE ORO: Ignorar los directorios "." (actual) y ".." (padre)
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        // 4. Construimos la ruta completa para imprimirla y usarla
        char ruta_completa[2048];
        snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", 
                 strcmp(ruta_base, "/") == 0 ? "" : ruta_base, // Evita doble barra "//" en la raíz
                 entrada->d_name);

        // Imprimimos la ruta del archivo o directorio que acabamos de encontrar
        printf("%s\n", ruta_completa);

        // 5. Si el elemento actual es un directorio (DT_DIR), entramos en él recursivamente
        if (entrada->d_type == DT_DIR) {
            listar_archivos(ruta_completa);
        }
    }

    // 6. Cerramos el directorio para liberar recursos
    closedir(dir);
}

int main() {
    printf("Iniciando el escaneo masivo desde la raíz (/)...\n");
    
    // Llamamos a nuestra función apuntando a la raíz del sistema
    listar_archivos("/");
    
    printf("Escaneo finalizado.\n");
    return EXIT_SUCCESS;
}
