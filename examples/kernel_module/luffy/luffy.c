#include <linux/module.h> // Required by all modules
#include <linux/fs.h>     // Required for the inode struct
#include <linux/kernel.h> // Required for KERN_INFO and printk
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
//
// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Iván Chavero");
MODULE_DESCRIPTION("Shows messages and creates its own device file");

#define DEVICE_NAME "luffy"
#define CLASS_NAME  "luffyclass"
#define BUFFER_SIZE 1024

static int exit = 0;
static int stream = 0;
static dev_t device_numbers;
static struct class *my_class;
static struct cdev my_cdev;

static int device_open_count = 0;
static char kernel_buffer[BUFFER_SIZE] = "El cielo resplandece a mi alrededoooor";
static char write_buffer[BUFFER_SIZE];

static int luffy_open(struct inode *, struct file *);
static int luffy_close(struct inode *, struct file *);
static ssize_t luffy_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t luffy_write(struct file *, const char *, size_t, loff_t *);

// Define file operations structure
static struct file_operations fops = {
    .open = luffy_open,
    .release = luffy_close,
    .read = luffy_read,
    .write = luffy_write
};

/**
 * @brief Called when data is read from the device file.
 */
static ssize_t luffy_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
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
static ssize_t luffy_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
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
static int luffy_open(struct inode *inode, struct file *file) {
    device_open_count++;
    printk(KERN_INFO "Se abrió el dispositivo: %s\n", DEVICE_NAME);
    return 0;
}

// Close function
static int luffy_close(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Cerrando disponsitivo Sonreiré el día de ho-ho-ho-ho-hoy\n");
    return 0;
}

/*
 * This function is called when the module is loaded.
 */
static int __init luffy_load(void)
{
    // printk is the kernel equivalent of printf
    // KERN_INFO is the log level
    printk(KERN_INFO "Ya llegó la luffy a jalarte las patas\n");

    /* 1. Allocate a device number (Major, Minor) */
    if (alloc_chrdev_region(&device_numbers, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT "luffy: Device Number could not be allocated\n");
        return -1;
    }
    printk(KERN_INFO "luffy: Device Numbers Major: %d, Minor: %d was registered\n",
           MAJOR(device_numbers), MINOR(device_numbers));

    /* 2. Create Device Class */
    if ((my_class = class_create(CLASS_NAME)) == NULL) {
        printk(KERN_ALERT "luffy: Class could not be created\n");
        goto ClassError;
    }

    // TIP: A partir de aquí es el equivalente a ejecutar mknod
    /* 3. Create Device file */
    if (device_create(my_class, NULL, device_numbers, NULL, DEVICE_NAME) == NULL) {
        printk(KERN_ALERT "luffy: Device could not be created\n");
        goto FileError;
    }

    /* 4. Initialize Device File */
    cdev_init(&my_cdev, &fops);

    /* 5. Register Device to Kernel */
    if (cdev_add(&my_cdev, device_numbers, 1) == -1) {
        printk(KERN_ALERT "luffy: Registering of device to kernel failed\n");
        goto AddError;
    }

    // A non-zero return value indicates failure, and the module won't load
    return 0;

    /* Error Handling */
    AddError:
        device_destroy(my_class, device_numbers);
    FileError:
        class_destroy(my_class);
    ClassError:
        unregister_chrdev_region(device_numbers, 1);
        return -1;
}

/*
 * This function is called when the module is removed.
 */
static void __exit luffy_unload(void)
{

    cdev_del(&my_cdev);
    device_destroy(my_class, device_numbers);
    class_destroy(my_class);
    unregister_chrdev_region(device_numbers, 1);
    printk(KERN_INFO "luffy: Goodbye, King of Pirates!!\n");

}

// Register the entry and exit functions
module_init(luffy_load);
module_exit(luffy_unload);


