#!/bin/bash

rm -rf ramdisk*

dd if=/dev/zero of=ramdisk bs=1M count=32

mkfs.ext2 -F ramdisk

mkdir -p ./initrd
mount -t ext2 ramdisk ./initrd -o loop

cp rootfs/* ./initrd -raf

umount ./initrd

gzip --best -c ramdisk > ramdisk.gz

mkimage -n "ramdisk" -A arm -O linux -T ramdisk -C gzip -d ramdisk.gz ramdisk.img
