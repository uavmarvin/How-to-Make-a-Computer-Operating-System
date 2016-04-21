How to Make a Computer Operating System
=======================================

[![Join the chat at https://gitter.im/uavmarvin/How-to-Make-a-Computer-Operating-System](https://badges.gitter.im/uavmarvin/How-to-Make-a-Computer-Operating-System.svg)](https://gitter.im/uavmarvin/How-to-Make-a-Computer-Operating-System?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/uavmarvin/How-to-Make-a-Computer-Operating-System.svg?branch=master)](https://travis-ci.org/uavmarvin/How-to-Make-a-Computer-Operating-System)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/8631/badge.svg)](https://scan.coverity.com/projects/uavmarvin-how-to-make-a-computer-operating-system)

### Quick Setup

Tested in Ubuntu 14.04 LTS (Trusty)

```Shell
git clone https://github.com/uavmarvin/How-to-Make-a-Computer-Operating-System.git
sudo apt-get install -y nasm qemu kpartx grub-pc
cd How-to-Make-a-Computer-Operating-System/src
make all
make run
```

#### Debug

**Kernel**
```Shell
cd src/kernel
gdb ./kernel.elf
target remote localhost:1234
```

**Helloworld**
```Shell
cd src/userland/helloworld
gdb ./hello
target remote localhost:1234
```

### More Information

**Source Code**: All the system source code will be stored in the [src](https://github.com/uavmarvin/How-to-Make-a-Computer-Operating-System/tree/master/src) directory. Each step will contain links to the different related files.

**Contributions**: Is is open to contributions, feel free to signal errors with issues or directly correct the errors with pull-requests.

**Questions**: Feel free to ask any questions by adding issues or commenting sections.

### What kind of OS are we building?

The goal is to build a very simple UNIX-based operating system in C++, not just a "proof-of-concept". The OS should be able to boot, start a userland shell, and be extensible.

![Screen](./preview.png)
