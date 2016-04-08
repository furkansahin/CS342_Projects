CONFIG_MODULE_SIG = n
CONFIG_MODULE_SIG_ALL=n
obj-m += simplemodule.o
all:
		gcc -Wall -o indexgen indexgen.c -lrt
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
