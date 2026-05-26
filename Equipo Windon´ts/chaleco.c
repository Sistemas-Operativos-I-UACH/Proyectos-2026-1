#include <linux/module.h> // Required by all modules
#include <linux/fs.h>     // Required for the inode struct
#include <linux/kernel.h> // Required for KERN_INFO and printk
//
// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ana, Daniel y Gibran");
MODULE_DESCRIPTION("Ropa de temu en kernel!!!.");

#define DEVICE_NAME "chaleco"
//#define CLASS_NAME  "chalecoclass"
#define BUFFER_SIZE 1024

static int exit = 0;
static int stream = 0;
static dev_t major_number;

static int device_open_count = 0;
static char kernel_buffer[BUFFER_SIZE] = "las mangas del chaleco";
static char write_buffer[BUFFER_SIZE];

static int chaleco_open(struct inode *, struct file *);
static int chaleco_close(struct inode *, struct file *);
static ssize_t chaleco_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t chaleco_write(struct file *, const char *, size_t, loff_t *);

// Define file operations structure
static struct file_operations fops = {
    .open = chaleco_open,
    .release = chaleco_close,
    .read = chaleco_read,
    .write = chaleco_write
};

/**
 * @brief Called when data is read from the device file.
 */
static ssize_t chaleco_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    int bytes_to_read = strlen(kernel_buffer);
    int errors = 0;

    if(exit == 0) {
        if ( stream ) {
            printk("Stream Activado");
        } else {
            printk("Stream Desactivado");
            exit = 1;
        }

        errors = copy_to_user(buf, kernel_buffer, bytes_to_read);
    } else {
        exit = 0;
        return 0;
    }

    if (errors != 0) {
        exit = 0;
        printk(KERN_ERR "chaleco: Failed to copy data to user.\n");
        return -EFAULT;
    } else {
        return bytes_to_read;
    }
}

/* Device write function */
static ssize_t chaleco_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    int bytes_written = 0;
    printk(KERN_INFO "Entrando a write\n");
    if(!stream) {
        stream = 1;
    } else {
        stream = 0;
    }

    if (*offset >= BUFFER_SIZE) return 0;

    if (len + *offset > BUFFER_SIZE) len = BUFFER_SIZE - *offset;

    if (copy_from_user(write_buffer + *offset, buffer, len) != 0) return -EFAULT;

    *offset += len;
    bytes_written = len;
    printk(KERN_INFO "chaleco: Received %d bytes from the user\n", bytes_written);
    printk(KERN_INFO "chaleco: Received %s from the user\n", write_buffer);
    printk(KERN_INFO "Saliendo a write\n");

    return bytes_written;
}

// Open function
static int chaleco_open(struct inode *inode, struct file *file) {
    device_open_count++;
    printk(KERN_INFO "Se abrió el dispositivo: %s\n", DEVICE_NAME);
    return 0;
}

// Close function
static int chaleco_close(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Cerrando dispositivo chaleco\n");
    return 0;
}

/*
 * This function is called when the module is loaded.
 */
static int __init chaleco_load(void)
{
    printk(KERN_INFO "Cargando el módulo chaleco\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "chaleco: Failed to register a major number\n");
        return major_number;
    }

    printk(KERN_INFO "Cargando Módulo chaleco!\n");
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", major_number);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major_number);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return 0;
}

/*
 * This function is called when the module is removed.
 */
static void __exit chaleco_unload(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Descargando módulo chaleco\n");
}

// Register the entry and exit functions
module_init(chaleco_load);
module_exit(chaleco_unload);
