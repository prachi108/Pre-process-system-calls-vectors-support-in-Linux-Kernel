#!/bin/sh
set -x
# WARNING: this script doesn't check for errors, so you have to enhance it in case any of the commands
# below fail.
rmmod syscalldev
insmod syscalldev.ko
rmmod syscall_vector1
insmod syscall_vector1.ko
rmmod syscall_vector2
insmod syscall_vector2.ko
lsmod
rm /dev/syscalldev
mknod /dev/syscalldev c 313 1
