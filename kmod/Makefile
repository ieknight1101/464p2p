obj-m += 464_p2p.o
464_p2p-objs := module.o 64_inbound.o 46_outbound.o 464_tables.o 464_netlink.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

all-debug:
	make CFLAGS="-D VERBOSE_464P2P" -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
install:
	insmod ./464_p2p.ko

uninstall:
	rmmod 464_p2p
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
fresh: clean all-debug