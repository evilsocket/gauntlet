INC_PATHS=-I/opt/local/include -I../include -I../include/core
CFLAGS= -w $(INC_PATHS)
FLAGS= -L/opt/local/lib -lpthread

all: 
	g++ -w -fPIC -c httptunnel.cpp
	g++ -shared -o ../httptunnel.so httptunnel.o
clean:
	rm -rf ../httptunnel.so
