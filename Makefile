obj-m += mychar.o
MY_CFLAGS += -g -DDEBUG 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) test_charDriver.c -o testdriver
debug:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
	EXTRA_CFLAGS="$(MY_CFLAGS)"
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
	rm testdriver 
