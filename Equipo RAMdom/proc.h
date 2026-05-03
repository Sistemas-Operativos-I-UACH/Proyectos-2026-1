/* 
*
* Estructura que representa a los campos del archivo /proc/pid/stat
* https://man7.org/linux/man-pages/man5/proc_pid_stat.5.html
*
*/
struct proc_stat {
    int pid;                      // 1
    char comm[256];               // 2
    char state;                   // 3
    int ppid;                     // 4
    int pgrp;                     // 5
    int session;                  // 6
    int tty_nr;                   // 7
    int tpgid;                    // 8
    unsigned int flags;           // 9
    unsigned long minflt;         // 10
    unsigned long cminflt;        // 11
    unsigned long majflt;         // 12
    unsigned long cmajflt;        // 13
    unsigned long utime;          // 14
    unsigned long stime;          // 15
    long cutime;                  // 16
    long cstime;                  // 17
    long priority;                // 18
    long nice;                    // 19
    long num_threads;             // 20
    long itrealvalue;             // 21
    unsigned long long starttime; // 22
    unsigned long vsize;          // 23
    long rss;                     // 24
    unsigned long rsslim;         // 25
    unsigned long startcode;      // 26
    unsigned long endcode;        // 27
    unsigned long startstack;     // 28
    unsigned long kstkesp;        // 29
    unsigned long kstkeip;        // 30
    unsigned long signal;         // 31
    unsigned long blocked;        // 32
    unsigned long sigignore;      // 33
    unsigned long sigcatch;       // 34
    unsigned long wchan;          // 35
    unsigned long nswap;          // 36
    unsigned long cnswap;         // 37
    int exit_signal;              // 38
    int processor;                // 39
    unsigned int rt_priority;     // 40
    unsigned int policy;          // 41
    unsigned long long delayacct_blkio_ticks; // 42
    unsigned long guest_time;     // 43
    long cguest_time;             // 44
    unsigned long start_data;     // 45
    unsigned long end_data;       // 46
    unsigned long start_brk;      // 47
    unsigned long arg_start;      // 48
    unsigned long arg_end;        // 49
    unsigned long env_start;      // 50
    unsigned long env_end;        // 51
    int exit_code;                // 52
};


int get_proc_info(struct proc_stat *, char *);
