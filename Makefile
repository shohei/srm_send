PROGRAM = srm_send
CC = g++
LIBUSB = libusb
LIBUSB1_0 = libusb-1.0
CFLAGS = `pkg-config --cflags --libs $(LIBUSB1_0)`

all:
	$(CC) -o $(PROGRAM) $(PROGRAM).cpp $(CFLAGS)
clean:
	rm $(PROGRAM) 
