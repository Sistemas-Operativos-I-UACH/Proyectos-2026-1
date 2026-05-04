#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include "proc.h"

const char proc_dir[] = "/proc";
int get_proc_info(struct proc_stat *stat_out, char *pid);

int main(int argc, char *argv[]) {
    struct dirent *entrada;
    struct proc_stat status;

    DIR *dir = opendir(proc_dir);

    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return EXIT_FAILURE;
    }
    // Imprimimos el encabezado de la tabla
    printf("%-8s %-8s %-25s %-10s\n", "PID", "PPID", "NOMBRE", "TIPO");
    printf("%-8s %-8s %-25s %-10s\n", "---", "----", "------", "----");

    while ((entrada = readdir(dir)) != NULL) {
        // d_name contiene el nombre del archivo o subdirectorio
        if ( !isdigit(entrada->d_name[0]) )
            continue;
        if ( get_proc_info(&status, entrada->d_name) == -1 ) {
        // Proceso fantasma que ya desapareció. Lo ignoramos y seguimos.
            continue;
        }
    // LÓGICA DE CLASIFICACIÓN:
    // Si el PID es 2 (kthreadd) o si su padre (PPID) es el proceso 2, es Kernel.
    char *tipo = (status.pid == 2 || status.ppid == 2) ? "Kernel" : "Usuario";
        
       printf("%-8d %-8d %-25s %-10s\n", 
               status.pid, 
               status.ppid, 
               status.comm, 
               tipo);
    }

    closedir(dir);

    return EXIT_SUCCESS;
}
int get_proc_info(struct proc_stat *stat_out, char *pid) {
    char buffer[512];
    char stat_filename[128];

    snprintf(stat_filename, sizeof(stat_filename), "/proc/%s/stat", pid);

    FILE *fp = fopen(stat_filename, "r");

    if (fp == NULL) {
        printf("No pude abrir el archivo de proceso: %s", stat_filename);
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        perror("Error al leer el archivo");
        fclose(fp);
        return -1;
    }
    fclose(fp);

    int parsed = sscanf(buffer,
        "%d (%[^)]) %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
        &stat_out->pid,
        stat_out->comm,
        &stat_out->state,
        &stat_out->ppid,
        &stat_out->pgrp,
        &stat_out->session,
        &stat_out->tty_nr,
        &stat_out->tpgid,
        &stat_out->flags,
        &stat_out->minflt,
        &stat_out->cminflt,
        &stat_out->majflt,
        &stat_out->cmajflt,
        &stat_out->utime,
        &stat_out->stime,
        &stat_out->cutime,
        &stat_out->cstime,
        &stat_out->priority,
        &stat_out->nice,
        &stat_out->num_threads,
        &stat_out->itrealvalue,
        &stat_out->starttime,
        &stat_out->vsize,
        &stat_out->rss,
        &stat_out->rsslim,
        &stat_out->startcode,
        &stat_out->endcode,
        &stat_out->startstack,
        &stat_out->kstkesp,
        &stat_out->kstkeip,
        &stat_out->signal,
        &stat_out->blocked,
        &stat_out->sigignore,
        &stat_out->sigcatch,
        &stat_out->wchan,
        &stat_out->nswap,
        &stat_out->cnswap,
        &stat_out->exit_signal,
        &stat_out->processor,
        &stat_out->rt_priority,
        &stat_out->policy,
        &stat_out->delayacct_blkio_ticks,
        &stat_out->guest_time,
        &stat_out->cguest_time,
        &stat_out->start_data,
        &stat_out->end_data,
        &stat_out->start_brk,
        &stat_out->arg_start,
        &stat_out->arg_end,
        &stat_out->env_start,
        &stat_out->env_end,
        &stat_out->exit_code
    );

    // Retorna el número de campos leídos con éxito
    return parsed;
}
