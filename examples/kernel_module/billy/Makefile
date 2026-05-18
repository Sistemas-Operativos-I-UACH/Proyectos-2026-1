# This is the object file that will be built from hello.c
obj-m += billy.o

# The 'all' target is the default
all:
	# Run the kernel's build system
	# -C: Change directory to the kernel build directory
	# M=$(PWD): Specify the location of our external module source
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# The 'clean' target removes build files
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
