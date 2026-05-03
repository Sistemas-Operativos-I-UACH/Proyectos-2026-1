#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include "proc.h"

const char ruta_proc[] = "/proc";

/* FUNCION PARA MOSTRAR LOS DATOS */
void mostrarProceso(struct proc_stat p, char *pid) {
    if (p.comm[0] == '[') {
        printf("Sistema -> ");
    } else {
        printf("Usuario -> ");
    }

    printf("PID: %s | PPID: %d | Nombre: %s\n",
           pid, p.ppid, p.comm);
}

/* FUNCION PARA LEER INFO DEL PROCESO */
int obtenerProceso(struct proc_stat *p, char *pid) {
    char buffer[512];
    char archivo[128];

    snprintf(archivo, sizeof(archivo), "/proc/%s/stat", pid);

    FILE *fp = fopen(archivo, "r");

    if (fp == NULL) {
        printf("No se pudo abrir el proceso %s\n", pid);
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        printf("Error al leer datos\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    sscanf(buffer,
        "%d (%[^)]) %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
        &p->pid,
        p->comm,
        &p->state,
        &p->ppid,
        &p->pgrp,
        &p->session,
        &p->tty_nr,
        &p->tpgid,
        &p->flags,
        &p->minflt,
        &p->cminflt,
        &p->majflt,
        &p->cmajflt,
        &p->utime,
        &p->stime,
        &p->cutime,
        &p->cstime,
        &p->priority,
        &p->nice,
        &p->num_threads,
        &p->itrealvalue,
        &p->starttime,
        &p->vsize,
        &p->rss,
        &p->rsslim,
        &p->startcode,
        &p->endcode,
        &p->startstack,
        &p->kstkesp,
        &p->kstkeip,
        &p->signal,
        &p->blocked,
        &p->sigignore,
        &p->sigcatch,
        &p->wchan,
        &p->nswap,
        &p->cnswap,
        &p->exit_signal,
        &p->processor,
        &p->rt_priority,
        &p->policy,
        &p->delayacct_blkio_ticks,
        &p->guest_time,
        &p->cguest_time,
        &p->start_data,
        &p->end_data,
        &p->start_brk,
        &p->arg_start,
        &p->arg_end,
        &p->env_start,
        &p->env_end,
        &p->exit_code
    );

    return 0;
}

int main() {
    DIR *carpeta;
    struct dirent *archivo;
    struct proc_stat proceso;

    int contador = 0;

    carpeta = opendir(ruta_proc);

    if (carpeta == NULL) {
        printf("No se pudo abrir /proc\n");
        return 1;
    }

    while ((archivo = readdir(carpeta)) != NULL) {

        /* validar que sea numero (PID) */
        if (isdigit(archivo->d_name[0]) == 0) {
            continue;
        }

        if (obtenerProceso(&proceso, archivo->d_name) == -1) {
            continue;
        }

        mostrarProceso(proceso, archivo->d_name);
        contador++;
    }

    closedir(carpeta);

    printf("\nTotal de procesos encontrados: %d\n", contador);

    return 0;
}
