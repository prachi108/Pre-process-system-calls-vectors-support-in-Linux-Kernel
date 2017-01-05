obj-m += syscalldev.o
obj-m += syscall_vector1.o
obj-m += syscall_vector2.o

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: syscall_modules svctl

syscall_modules:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

svctl: svctl.c
	gcc -Wall -Werror -I$(INC)/../../../include svctl.c -o svctl

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f svctl
