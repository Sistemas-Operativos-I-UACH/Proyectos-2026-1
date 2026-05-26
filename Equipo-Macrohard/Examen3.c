#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/minmax.h>
#include <linux/mutex.h>
#include <linux/version.h>

// Metadatos del módulo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sistemas operativos");
MODULE_DESCRIPTION("Módulo de telemetría y comunicaciones - Proyecto Orion");

#define DEVICE_NAME "orion"
#define CLASS_NAME  "orion_class"
#define BUFFER_SIZE 1024

static int exit_flag = 0;
static int stream = 0; // 0 = Desactivado (Default), 1 = Activado
static dev_t device_numbers;
static struct class *my_class;
static struct cdev my_cdev;

static int device_open_count = 0;
// Mensaje para el espacio de usuario
static char kernel_buffer[BUFFER_SIZE] = "[ORION] Enlace de datos estable. Telemetría nominal.\n";
static char write_buffer[BUFFER_SIZE];

// Mutex para protección de concurrencia
static DEFINE_MUTEX(orion_mutex);

static int orion_open(struct inode *, struct file *);
static int orion_close(struct inode *, struct file *);
static ssize_t orion_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t orion_write(struct file *, const char __user *, size_t, loff_t *);

// Estructura de operaciones de archivo
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = orion_open,
    .release = orion_close,
    .read = orion_read,
    .write = orion_write
};

/* Función de lectura (Espacio de Usuario <- Kernel) */
static ssize_t orion_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    ssize_t bytes_to_read;
    int errors = 0;

    // Limitar al tamaño del buffer del usuario
    bytes_to_read = min(len, (size_t)strlen(kernel_buffer));

    if (exit_flag == 0) {
        if (!stream) {
            // Si el stream está desactivado, preparamos la salida para la siguiente iteración
            exit_flag = 1;
        }

        errors = copy_to_user(buf, kernel_buffer, bytes_to_read);

        if (errors != 0) {
            printk(KERN_ERR "orion: Fallo al copiar datos al espacio de usuario.\n");
            return -EFAULT;
        }
        return bytes_to_read;
    } else {
        // Reiniciar la bandera para la próxima vez que se lea el archivo (e.g., con 'cat')
        exit_flag = 0;
        return 0; // EOF
    }
}

/* Función de escritura (Espacio de Usuario -> Kernel) */
static ssize_t orion_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    size_t bytes_to_copy;

    // Seguridad: Evitar desbordamiento de búfer
    bytes_to_copy = min(len, (size_t)(BUFFER_SIZE - 1));

    mutex_lock(&orion_mutex);

    if (copy_from_user(write_buffer, buffer, bytes_to_copy) != 0) {
        mutex_unlock(&orion_mutex);
        return -EFAULT;
    }

    // Seguridad: Asegurar la terminación en nulo para poder usar %s en printk de manera segura
    write_buffer[bytes_to_copy] = '\0';

    // Evaluar la directiva enviada desde el espacio de usuario
    if (write_buffer[0] == '1') {
        stream = 1;
        exit_flag = 0; // Reiniciar para que la lectura funcione inmediatamente
        printk(KERN_INFO "orion: Modo flujo ACTIVADO. Transmitiendo señal continua...\n");
    } else if (write_buffer[0] == '0') {
        stream = 0;
        printk(KERN_INFO "orion: Modo flujo DESACTIVADO.\n");
    } else {
        // Mostrar la cadena recibida en la bitácora
        printk(KERN_INFO "orion: Cadena recibida desde control: %s\n", write_buffer);
    }

    mutex_unlock(&orion_mutex);
    return bytes_to_copy;
}

/* Función de apertura */
static int orion_open(struct inode *inode, struct file *file) {
    mutex_lock(&orion_mutex);
    device_open_count++;
    printk(KERN_INFO "orion: Dispositivo abierto. Sesiones activas: %d\n", device_open_count);
    mutex_unlock(&orion_mutex);
    return 0;
}

/* Función de cierre */
static int orion_close(struct inode *inode, struct file *file) {
    mutex_lock(&orion_mutex);
    device_open_count--;
    printk(KERN_INFO "orion: Conexión cerrada. Sesiones activas: %d\n", device_open_count);
    mutex_unlock(&orion_mutex);
    return 0;
}

/* Inicialización del módulo */
static int __init orion_load(void)
{
    printk(KERN_INFO "orion: Iniciando secuencia de carga del módulo de comunicaciones.\n");

    /* 1. Asignar número de dispositivo (Major, Minor) */
    if (alloc_chrdev_region(&device_numbers, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT "orion: No se pudo asignar el número de dispositivo.\n");
        return -1;
    }
    printk(KERN_INFO "orion: Dispositivo registrado - Major: %d, Minor: %d\n",
           MAJOR(device_numbers), MINOR(device_numbers));

    /* 2. Crear clase de dispositivo */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    my_class = class_create(CLASS_NAME);
#else
    my_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
    if (IS_ERR(my_class)) {
        printk(KERN_ALERT "orion: No se pudo crear la clase.\n");
        goto ClassError;
    }

    /* 3. Crear archivo de dispositivo dinámicamente en /dev/ */
    if (device_create(my_class, NULL, device_numbers, NULL, DEVICE_NAME) == NULL) {
        printk(KERN_ALERT "orion: No se pudo crear el archivo de dispositivo.\n");
        goto FileError;
    }

    /* 4. Inicializar archivo de dispositivo */
    cdev_init(&my_cdev, &fops);

    /* 5. Registrar dispositivo en el Kernel */
    if (cdev_add(&my_cdev, device_numbers, 1) == -1) {
        printk(KERN_ALERT "orion: Falló el registro del dispositivo en el kernel.\n");
        goto AddError;
    }

    return 0;

    /* Manejo de errores (Rollback) */
    AddError:
        device_destroy(my_class, device_numbers);
    FileError:
        class_destroy(my_class);
    ClassError:
        unregister_chrdev_region(device_numbers, 1);
        return -1;
}

/* Descarga del módulo */
static void __exit orion_unload(void)
{
    cdev_del(&my_cdev);
    device_destroy(my_class, device_numbers);
    class_destroy(my_class);
    unregister_chrdev_region(device_numbers, 1);
    printk(KERN_INFO "orion: Módulo descargado. Fin de la transmisión.\n");
}

module_init(orion_load);
module_exit(orion_unload);