#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int esNumero(char *str) {
    for(int i = 0; str[i]; i++) {
        if(!isdigit(str[i])) return 0;
    }
    return 1;
}

int main() {
    DIR *dir;
    struct dirent *ent;

    dir = opendir("/proc");

    if(dir == NULL) {
        printf("Error al abrir /proc\n");
        return 1;
    }

    while((ent = readdir(dir)) != NULL) {
        if(esNumero(ent->d_name)) {

            char ruta[256];
            sprintf(ruta, "/proc/%s/stat", ent->d_name);

            FILE *f = fopen(ruta, "r");
            if(f == NULL) continue;

            int pid, ppid;
            char nombre[256];

            // Leer datos básicos
            fscanf(f, "%d (%[^)]) %*c %d", &pid, nombre, &ppid);
            fclose(f);

            // Verificar si es kernel o usuario
            char rutaExe[256];
            sprintf(rutaExe, "/proc/%d/exe", pid);

            if(access(rutaExe, F_OK) == 0) {
                printf("PID: %d | PPID: %d | Nombre: %s | Usuario\n", pid, ppid, nombre);
            } else {
                printf("PID: %d | PPID: %d | Nombre: %s | Kernel\n", pid, ppid, nombre);
            }
        }
    }

    closedir(dir);
    return 0;
}
