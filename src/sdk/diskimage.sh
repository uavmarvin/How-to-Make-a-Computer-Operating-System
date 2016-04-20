#!/bin/bash
qemu-img create c.img 20M
#dd if=/dev/zero of=c.img count=40320
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
a
1
w
EOF

fdisk -l -u ./c.img
sudo kpartx -av ./c.img
sudo mkfs.ext2 /dev/mapper/loop0p1
sudo mkdir -p /mnt/img
sudo mount /dev/mapper/loop0p1 /mnt/img
sudo grub-install --no-floppy --root-directory=/mnt/img --boot-directory=/mnt/img/boot /dev/loop0
sudo cp -R bootdisk/* /mnt/img
sudo umount /mnt/img
sudo rmdir /mnt/img
sudo kpartx -d ./c.img
