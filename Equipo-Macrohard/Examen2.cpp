#include <iostream>
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <string>

//Jesus Gerardo Fernandez Quintero 374392
//Luis Raymundo Rodriguez Luna     199761
//Franklyn Jesus Robles Venzor     377119

using namespace std;

// Secuencias ANSI para renderizar colores y estilos en la terminal (espacio de usuario)
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string BLUE = "\033[34m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN = "\033[36m";

bool esNumero(const char* str) {
    // Lógica en espacio de usuario para validar que los nombres de los directorios
    // extraídos del VFS (Virtual File System) contengan únicamente dígitos.
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool esKernelProceso(const string& pid) {
    // procfs es un pseudo-sistema de archivos que reside en RAM, no en disco.
    // El archivo 'cmdline' contiene los argumentos pasados a la llamada al sistema execve()
    // cuando el proceso fue creado y reemplazó su imagen en memoria.
    string ruta = "/proc/" + pid + "/cmdline";
    ifstream archivo(ruta.c_str());

    if (!archivo.is_open()) return false;

    char c;
    // Si la lectura obtiene al menos un byte, significa que el proceso tiene un entorno
    // y argumentos de usuario (llamó a execve).
    if (archivo.get(c)) {
        archivo.close();
        return false; 
    }

    // Los hilos del kernel (kthreads) son creados directamente por el kernel usando
    // llamadas internas como kernel_thread(). Dado que nunca invocan execve() 
    // ni tienen espacio de direcciones de usuario, su archivo cmdline siempre está vacío.
    archivo.close();
    return true; 
}

int main() {
    DIR* dir;
    struct dirent* ent;

    // opendir() envoca llamadas al sistema (típicamente openat() y fstat()) para obtener
    // un descriptor de archivo que apunta al directorio raíz del procfs.
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error de E/S al intentar acceder a procfs");
        return 1;
    }

    // Formateo del encabezado en la salida estándar (stdout)
    cout << BOLD << CYAN
         << "========================================================================\n"
         << left
         << setw(5)  << ""
         << setw(10) << "PID"
         << setw(10) << "PPID"
         << setw(15) << "ORIGEN"
         << setw(30) << "NOMBRE DEL PROCESO"
         << "\n========================================================================"
         << RESET << endl;

    // readdir() actúa como una envoltura (wrapper) en glibc para la llamada al sistema getdents64(),
    // la cual pide al kernel que lea las estructuras 'dentry' (entradas de directorio).
    while ((ent = readdir(dir)) != NULL) {
        
        // DT_DIR verifica a nivel del sistema de archivos que el inodo es un directorio.
        // Si además el nombre es numérico, representa el PID de un 'task_struct' (la 
        // estructura principal que usa el kernel para representar procesos e hilos).
        if (ent->d_type == DT_DIR && esNumero(ent->d_name)) {
            string pid = ent->d_name;
            
            // El archivo 'status' es generado dinámicamente por el kernel al ser leído,
            // exponiendo los metadatos del task_struct en un formato legible por humanos.
            string ruta = "/proc/" + pid + "/status";

            ifstream archivo(ruta.c_str());
            string linea;
            string nombre, ppid;

            if (archivo.is_open()) {
                // Se realiza un parsing en espacio de usuario del flujo de datos del kernel
                while (getline(archivo, linea)) {
                    if (linea.find("Name:") == 0) {
                        nombre = linea.substr(6);
                        // Limpieza de tabulaciones o espacios residuales entregados por procfs
                        size_t inicio = nombre.find_first_not_of(" \t");
                        if (inicio != string::npos) nombre = nombre.substr(inicio);
                    } else if (linea.find("PPid:") == 0) {
                        ppid = linea.substr(6);
                        size_t inicio = ppid.find_first_not_of(" \t");
                        if (inicio != string::npos) ppid = ppid.substr(inicio);
                    }

                    // Optimización: romper el ciclo de lectura (I/O) tan pronto como
                    // tengamos la información necesaria de este task_struct.
                    if (!nombre.empty() && !ppid.empty()) break;
                }
                archivo.close();

                bool esKernel = esKernelProceso(pid);

                // Lógica de renderizado en terminal: 
                // Asigna colores dependiendo del contexto de ejecución del proceso.
                string colorTipo = esKernel ? YELLOW : GREEN;
                string tipoStr = esKernel ? "[Kernel]" : "[Usuario]";

                cout << BOLD << BLUE << " ->  " << RESET
                     << left
                     << setw(10) << pid
                     << setw(10) << ppid
                     << colorTipo << setw(15) << tipoStr << RESET
                     << setw(30) << nombre
                     << endl;
            }
        }
    }

    // Se invoca la llamada al sistema close() para liberar el descriptor de archivo 
    // y las estructuras de memoria asignadas por el kernel para la iteración del directorio.
    closedir(dir);
    return 0;
}
