all: use-libudev use-libusb

use-libudev: main.c hid.c
	gcc -o $@ $^ -ludev

use-libusb: main.c hid-libusb.c
	gcc -o $@ $^ -I/usr/include/libusb-1.0 -pthread -lusb-1.0

clean:
	rm -rf use*
