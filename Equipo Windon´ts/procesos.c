#include <stdio.h>   //para usar printf, fopen, etc
#include <stdlib.h>  //funciones generales
#include <dirent.h>  //para manejar directorios (/proc)
#include <string.h>  //para manejar cadenas

//funcion para verificar si el nombre es numero (PID)
int esNumero(const char *str) {
    while (*str) {
        if (*str < '0' || *str > '9')
            return 0;  //no es numero 
        str++;
    }
    return 1;  //si es numero 
}

int main() {
    DIR *dir;
    struct dirent *entrada;

    //abrir el directorio /proc
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("No se pudo abrir /proc");
        return 1;
    }

    printf("%-8s %-8s %-20s %-10s\n", "PID", "PPID", "Nombre", "Tipo");

   //leer cada archivo/directorio dentro del /proc
    while ((entrada = readdir(dir)) != NULL) {
        //solo tomar los que son numeros (procesos)
        if (esNumero(entrada->d_name)) {

            char ruta[256];
            //construir ruta: /proc/PID/status
            sprintf(ruta, "/proc/%s/status", entrada->d_name);

            FILE *f = fopen(ruta, "r");
            if (f == NULL)
                continue;

            char linea[256];
            int pid = 0, ppid = 0;
            char nombre[100] = "";
           //leer el archivo linea por linea 
            while (fgets(linea, sizeof(linea), f)) {

                if (strncmp(linea, "Name:", 5) == 0)
                    sscanf(linea, "Name: %s", nombre);

                if (strncmp(linea, "Pid:", 4) == 0)
                    sscanf(linea, "Pid: %d", &pid);

                if (strncmp(linea, "PPid:", 5) == 0)
                    sscanf(linea, "PPid: %d", &ppid);
            }

            fclose(f);

            // Clasificación (aproximada)
            char tipo[20];
            if (ppid == 2 || pid == 2)
                strcpy(tipo, "Kernel");
            else
                strcpy(tipo, "Usuario");
           // mostrar resultados
            printf("%-8d %-8d %-20s %-10s\n", pid, ppid, nombre, tipo);
        }
    }

    closedir(dir);
    return 0;
}
