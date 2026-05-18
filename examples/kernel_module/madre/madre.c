#include <linux/module.h> // Required by all modules
#include <linux/fs.h>     // Required for the inode struct
#include <linux/kernel.h> // Required for KERN_INFO and printk
//
// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Iván Chavero");
MODULE_DESCRIPTION("Billy Kernel Module biatch!!!.");

#define DEVICE_NAME "madre"
//#define CLASS_NAME  "madreclass"
#define BUFFER_SIZE 1024

static int exit = 0;
static int stream = 0;
static dev_t major_number;

static int device_open_count = 0;
static char kernel_buffer[BUFFER_SIZE] = "El cielo resplandece a mi alrededoooor";
static char write_buffer[BUFFER_SIZE];

static int madre_open(struct inode *, struct file *);
static int madre_close(struct inode *, struct file *);
static ssize_t madre_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t madre_write(struct file *, const char *, size_t, loff_t *);

// Define file operations structure
static struct file_operations fops = {
    .open = madre_open,
    .release = madre_close,
    .read = madre_read,
    .write = madre_write
};

/**
 * @brief Called when data is read from the device file.
 */
static ssize_t madre_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
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
        printk(KERN_ERR "mydevice: Failed to copy data to user.\n");
        return -EFAULT; // Bad address error
    } else {
        return bytes_to_read;
    }

}

/* Device write function */
static ssize_t madre_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
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
    printk(KERN_INFO "chardev: Received %d bytes from the user\n", bytes_written);
    printk(KERN_INFO "chardev: Received %s from the user\n", write_buffer);
    printk(KERN_INFO "Saliendo a write\n");

    return bytes_written;

}
// Open function
static int madre_open(struct inode *inode, struct file *file) {
    device_open_count++;
    printk(KERN_INFO "Se abrió el dispositivo: %s\n", DEVICE_NAME);
    return 0;
}

// Close function
static int madre_close(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Cerrando disponsitivo Sonreiré el día de ho-ho-ho-ho-hoy\n");
    return 0;
}

/*
 * This function is called when the module is loaded.
 */
static int __init madre_load(void)
{
    // printk is the kernel equivalent of printf
    // KERN_INFO is the log level
    printk(KERN_INFO "Ya llegó la madre a jalarte las patas\n");


    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "simple_module: Failed to register a major number\n");
        return major_number;
    }

    printk(KERN_INFO "Cargando Módulo Chirisco!\n");
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", major_number);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major_number);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");


    // A non-zero return value indicates failure, and the module won't load
    return 0;
}

/*
 * This function is called when the module is removed.
 */
static void __exit madre_unload(void)
{
    printk(KERN_INFO "No le saque!!\n");
}

// Register the entry and exit functions
module_init(madre_load);
module_exit(madre_unload);


