#include <linux/module.h> // Required by all modules
#include <linux/kernel.h> // Required for KERN_INFO and printk

/*
 * This function is called when the module is loaded.
 */
static int __init billy_start(void)
{
    // printk is the kernel equivalent of printf
    // KERN_INFO is the log level
    printk(KERN_INFO "Billy jean is not my girl\n");

    // A non-zero return value indicates failure, and the module won't load
    return 1;
}

/*
 * This function is called when the module is removed.
 */
static void __exit billy_end(void)
{
    printk(KERN_INFO "She's just a girl!\n");
}

// Register the entry and exit functions
module_init(billy_start);
module_exit(billy_end);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Iván Chavero");
MODULE_DESCRIPTION("Billy Kernel Module biatch!!!.");
