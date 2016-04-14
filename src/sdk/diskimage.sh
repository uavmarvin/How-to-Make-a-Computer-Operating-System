#!/bin/bash
#qemu-img create c.img 2M
dd if=/dev/zero of=c.img count=40320
fdisk ./c.img  << EOF
x
s
63
h
16
c
40
r
n
p
1
2048
40319
t
83
w
EOF

fdisk -l -u ./c.img
sudo losetup -o 1048576 /dev/loop1 ./c.img

sudo mkfs.ext2 /dev/loop1
sudo mkdir -p /mnt/loop1
sudo mount /dev/loop1 /mnt/loop1
sudo cp -R bootdisk/* /mnt/loop1
sudo umount /mnt/loop1
sudo rmdir /mnt/loop1
sudo losetup -d /dev/loop1
grub --device-map=/dev/null << EOF
device (hd0) ./c.img
geometry (hd0) 40 16 63
root (hd0,0)
setup (hd0)
quit
EOF
