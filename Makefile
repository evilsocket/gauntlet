INC_PATHS=-I/opt/local/include -Inetclass
CFLAGS= -w $(INC_PATHS)
FLAGS= -L/opt/local/lib -lpthread

OBJS = log.o \
	   config.o \
	   socket.o \
	   thread.o \
	   listener.o 
       

.cpp.o: 
	g++ -c $(CFLAGS) $< 
all: $(OBJS)
	g++ *.o main.cpp -o gauntlet $(CFLAGS) $(LFLAGS)
clean:
	rm -rf *.o gauntlet
