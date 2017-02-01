CC = g++

CFLAGS = -std=c++1y -O2 -g3 
INCFLAGS = -I/usr/include -I/usr/local/include 
LDFLAGS = -L/usr/lib -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_photo

RM = /bin/rm -f 
all:  hdr
hdr: test.o
	$(CC) -o hdr ./build/test.o $(INCFLAGS) $(LDFLAGS) 
test.o: ./src/test.cpp
	mkdir -p ./build
	$(CC) $(CFLAGS) $(INCFLAGS) -Wall -c -o ./build/test.o ./src/test.cpp
directories: ${MKDIR_P} ./build
clean: 
	$(RM) -r ./build/ hdr 

