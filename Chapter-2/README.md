## Chapter 2: Setup the development environment

The first step is to setup a good and viable development environment. Using Virtualbox, you'll be able to compile and test your OS from all the OSs (Linux, Windows or Mac).

### Install Virtualbox

> Oracle VM VirtualBox is a virtualization software package for x86 and AMD64/Intel64-based computers.

Download and install it for your system at https://www.virtualbox.org/wiki/Downloads.

### Start and test your development environment

Once Virtualbox is installed, you need to download the Ubuntu Trusty image at http://www.ubuntu.com/download.

Create new virtual machine in your Virtualbox and install Ubuntu Trusty with image.

#### Build and test our operating system

The file [**Makefile**](https://github.com/uavmarvin/How-to-Make-a-Computer-Operating-System/blob/master/src/Makefile) defines some basics rules for building the kernel, the user libc and some userland programs.

Build:

```
make all
```

Test our operating system with qemu:

```
make run
```

The documentation for qemu is available at [QEMU Emulator Documentation](http://wiki.qemu.org/download/qemu-doc.html).

You can exit the emulator using: Ctrl-a.
