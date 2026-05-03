#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

// Definición de la bandera del kernel (obtenida de include/linux/sched.h)
#define PF_KTHREAD 0x00200000 

const char proc_dir[] = "/proc";

// Definimos la estructura con todos los tipos correspondientes al sscanf
struct proc_stat {
    int pid;
    char comm[256];
    char state;
    int ppid;
    int pgrp;
    int session;
    int tty_nr;
    int tpgid;
    unsigned int flags;  // ¡Este es el campo 9 que nos dirá si es del kernel!
    unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
    long cutime, cstime, priority, nice, num_threads, itrealvalue;
    unsigned long long starttime;
    unsigned long vsize; // Campo 23: tamaño de memoria virtual
    long rss;
    unsigned long rsslim, startcode, endcode, startstack, kstkesp, kstkeip;
    unsigned long signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap;
    int exit_signal, processor;
    unsigned int rt_priority, policy;
    unsigned long long delayacct_blkio_ticks;
    unsigned long guest_time;
    long cguest_time;
    unsigned long start_data, end_data, start_brk, arg_start, arg_end, env_start, env_end;
    int exit_code;
};

// Prototipo de la función
int get_proc_info(struct proc_stat *stat_out, char *pid);

int main(int argc, char *argv[]) {
    struct dirent *entrada;
    struct proc_stat status;

    DIR *dir = opendir(proc_dir);

    if (dir == NULL) {
        perror("Error al abrir el directorio /proc");
        return EXIT_FAILURE;
    }

    // Encabezado de la tabla
    printf("%-10s %-10s %-30s %-15s\n", "PID", "PPID", "NOMBRE", "TIPO");
    printf("%-10s %-10s %-30s %-15s\n", "---", "----", "------", "----");

    while ((entrada = readdir(dir)) != NULL) {
        // Ignorar todo lo que no empiece con un número (ya que los PIDs son números)
        if ( !isdigit(entrada->d_name[0]) )
            continue;

        if ( get_proc_info(&status, entrada->d_name) == -1 ) {
            // Algunos procesos pueden desaparecer mientras leemos, lo ignoramos y seguimos
            continue;
        }

        // Lógica de detección: Es de Kernel si tiene la bandera PF_KTHREAD,
        // o si su padre es kthreadd (PID 2), o si es el propio kthreadd.
        char tipo[15] = "Usuario";
        if ( (status.flags & PF_KTHREAD) || status.ppid == 2 || status.pid == 2 ) {
            strcpy(tipo, "Kernel");
        }

        // Imprimimos la información formateada
        printf("%-10s %-10d %-30s %-15s\n",
               entrada->d_name,
               status.ppid,
               status.comm,
               tipo);
    }

    closedir(dir);
    return EXIT_SUCCESS;
}

/*
* Lee y extrae la información de un archivo de proceso del directorio proc
*/
int get_proc_info(struct proc_stat *stat_out, char *pid) {
    char buffer[1024]; // Aumentamos un poco el buffer para mayor seguridad
    char stat_filename[256];

    snprintf(stat_filename, sizeof(stat_filename), "/proc/%s/stat", pid);

    FILE *fp = fopen(stat_filename, "r");

    if (fp == NULL) {
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    // Tu sscanf exacto para capturar los 52 campos que documenta man7
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

    return parsed;
}
