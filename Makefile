INC_PATHS=-I/opt/local/include -Iinclude -Iinclude/core
CFLAGS= -w $(INC_PATHS)
FLAGS= -L/opt/local/lib -lpthread

OBJS = src/core/log.o \
	   src/core/config.o \
	   src/core/socket.o \
	   src/core/thread.o \
	   src/core/listener.o \
	   src/gtunnel.o \
       src/main.o

.cpp.o: 
	g++ -c $(CFLAGS) $< 
all: $(OBJS)
	g++ *.o -o gauntlet $(CFLAGS) $(LFLAGS)
	cd tunnels && make
clean:
	rm -rf *.o gauntlet
	cd tunnels && make clean
