# Proyectos Sistemas Operativos I 2025-2

# Test Kernel Module
This is a test kernel module

# Module building

## Build dependencies

On a Fedora system install the `Development Tools`  and the `C Development Tools and Libraries`.

```
~$ sudo dnf groupinstall -y 'Development Tools' 'C Development Tools and Libraries'
~$ sudo dnf install -y kernel-headers kernel-devel
```

On Debian based systems:

```
~$ sudo apt install -y build-essential linux-headers-$(uname -r)
```

## Build module

**Enter the module directory**

```bash
~$ cd catrina
```

The Makefile defines how to build the module, execute the `make` command to compile the module.

```bash
catrina $ make
```

## Load module

Load the module using the `insmod` command.

```bash
~$ sudo insmod catrina.ko
```

### Check if the module has been loaded

```bash
~$ sudo dmesg -w
```

## Check module major number

The major number is needed for creating the device that is used to interact with the module
```bash
~$ journalctl -f | grep mknod
```


## Remove module

Remove the module using the `rmmod` command

```bash
~$ sudo rmmod catrina
```

## Create the module device

This module registers a device major number but it does not create the device. Create the `/dev/super_module` device manually.

```bash
~$ sudo mknod /dev/catrina c <device_major_number> 0
```

### Test the module

Read from the module

```
~$ cat /dev/catrina
```

Write to the module

```
~$ echo 1 > /dev/catrina
```

### Git Basic commands

**Clone this repository**

```
~$ git clone git@github.com:Sistemas-Operativos-I-UACH/Proyectos-2025-2.git
```

**Use a branch other than *main***

```
~$ git checkout *yourbranch*
```

**Add a file to next commit**

Add the `README.md` file to the commit:

```
~$ git add README.md
```

**Commit a the changes**
```
~$ git commit -m 'commit message'
```

or to open your favorite editor and add a more extense commit message:

```
~$ git commit
```

**Push changes to our public github repository**

This team is using github.com as the public repository and the *Master Jedi* repository as the main one.

```
~$ git  push
```

**Get and integrate the latest changes of the current branch from the public git repository**

```
~$ git pull
```

**Get all changes in all the branches**

```
~$ git fetch --all
```

Once you get the changes you must pull them to integrate them in the branch

```
~$ git checkout your_branch
~$ git pull
```


***Be happy***

# References

* https://education.github.com/git-cheat-sheet-education.pdf
* https://code.visualstudio.com/docs/editor/versioncontrol
* https://www.thegeekstuff.com/2013/07/write-linux-kernel-module/

