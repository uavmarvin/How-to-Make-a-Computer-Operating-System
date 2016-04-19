How to Make a Computer Operating System
=======================================

[![Join the chat at https://gitter.im/uavmarvin/How-to-Make-a-Computer-Operating-System](https://badges.gitter.im/uavmarvin/How-to-Make-a-Computer-Operating-System.svg)](https://gitter.im/uavmarvin/How-to-Make-a-Computer-Operating-System?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[![Build Status](https://travis-ci.org/uavmarvin/How-to-Make-a-Computer-Operating-System.svg?branch=master)](https://travis-ci.org/uavmarvin/How-to-Make-a-Computer-Operating-System)

### Quick Setup

```Shell
git clone https://github.com/uavmarvin/How-to-Make-a-Computer-Operating-System.git
sudo apt-get install nasm qemu
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

Online book about how to write a computer operating system in C/C++ from scratch.

**Caution**: This repository is a remake of my old course. It was written several years ago [as one of my first projects when I was in High School](https://github.com/SamyPesse/devos), I'm still refactoring some parts. The original course was in French and I'm not an English native. I'm going to continue and improve this course in my free-time.

**Book**: An online version is available at [http://samypesse.gitbooks.io/how-to-create-an-operating-system/](http://samypesse.gitbooks.io/how-to-create-an-operating-system/) (PDF, Mobi and ePub). It was generated using [GitBook](https://www.gitbook.com/).

**Source Code**: All the system source code will be stored in the [src](https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System/tree/master/src) directory. Each step will contain links to the different related files.

**Contributions**: This course is open to contributions, feel free to signal errors with issues or directly correct the errors with pull-requests.

**Questions**: Feel free to ask any questions by adding issues or commenting sections.

You can follow me on Twitter [@SamyPesse](https://twitter.com/SamyPesse) or [GitHub](https://github.com/SamyPesse).

### What kind of OS are we building?

The goal is to build a very simple UNIX-based operating system in C++, not just a "proof-of-concept". The OS should be able to boot, start a userland shell, and be extensible.

![Screen](./preview.png)
