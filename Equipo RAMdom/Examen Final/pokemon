#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Garcia - Cuayahui - Gallegos");
MODULE_DESCRIPTION("Modulo de Kernel estilo Pokemon");

#define DEVICE_NAME "poke_device"
#define BUFFER_SIZE 1024

static dev_t major_number;

static int stream = 0;

static char kernel_buffer[BUFFER_SIZE] =
"Un Gastly salvaje aparecio en el kernel!\n";

static char write_buffer[BUFFER_SIZE];

/* Prototipos */
static int poke_open(struct inode *, struct file *);
static int poke_close(struct inode *, struct file *);
static ssize_t poke_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t poke_write(struct file *, const char __user *, size_t, loff_t *);

/* Operaciones del archivo */
static struct file_operations fops = {
    .open = poke_open,
    .release = poke_close,
    .read = poke_read,
    .write = poke_write
};

/* READ */
static ssize_t poke_read(struct file *filep,
                         char __user *buffer,
                         size_t len,
                         loff_t *offset)
{
    int bytes_to_read;
    int errors;

    bytes_to_read = strlen(kernel_buffer);

    if (*offset >= bytes_to_read)
    {
        *offset = 0;
        return 0;
    }

    errors = copy_to_user(buffer, kernel_buffer, bytes_to_read);

    if (errors != 0)
    {
        printk(KERN_ERR "Error al copiar datos al usuario\n");
        return -EFAULT;
    }

    *offset += bytes_to_read;

    if (stream)
    {
        printk(KERN_INFO "Modo flujo Pokemon ACTIVADO\n");
    }

    return bytes_to_read;
}

/* WRITE */
static ssize_t poke_write(struct file *filep,
                          const char __user *buffer,
                          size_t len,
                          loff_t *offset)
{
    int bytes_written;

    if (*offset >= BUFFER_SIZE)
        return 0;

    if (len + *offset > BUFFER_SIZE)
        len = BUFFER_SIZE - *offset;

    if (copy_from_user(write_buffer + *offset, buffer, len) != 0)
    {
        return -EFAULT;
    }

    *offset += len;
    bytes_written = len;

    write_buffer[len] = '\0';

    printk(KERN_INFO "Mensaje recibido desde usuario: %s\n", write_buffer);

    /* Activar o desactivar stream */
    if (write_buffer[0] == '1')
    {
        stream = 1;
        printk(KERN_INFO "Modo flujo ACTIVADO\n");
    }
    else if (write_buffer[0] == '0')
    {
        stream = 0;
        printk(KERN_INFO "Modo flujo DESACTIVADO\n");
    }

    return bytes_written;
}

/* OPEN */
static int poke_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Entrenador conectado al dispositivo Pokemon\n");
    return 0;
}

/* CLOSE */
static int poke_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Entrenador desconectado del dispositivo Pokemon\n");
    return 0;
}

/* LOAD */
static int __init poke_load(void)
{
    printk(KERN_INFO "El modulo Pokemon fue cargado correctamente\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0)
    {
        printk(KERN_ALERT "No se pudo registrar el dispositivo\n");
        return major_number;
    }

    printk(KERN_INFO "Dispositivo registrado correctamente\n");
    printk(KERN_INFO "Major number asignado: %d\n", major_number);

    printk(KERN_INFO
           "Crear dispositivo con:\n");

    printk(KERN_INFO
           "mknod /dev/%s c %d 0\n",
           DEVICE_NAME,
           major_number);

    return 0;
}

/* UNLOAD */
static void __exit poke_unload(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "El modulo Pokemon fue removido correctamente\n");
}

module_init(poke_load);
module_exit(poke_unload);
