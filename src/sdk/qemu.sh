#!/bin/bash
qemu-system-x86_64 -m 1024 -s -hda ./c.img  -curses -serial /dev/tty  -redir tcp:2323::23
