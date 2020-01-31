
LIBRARIES = -lpthread
APP_NAME  = udp_server
COMPILER  = gcc
FLAGS     = -Wall


all: 
	$(COMPILER) $(LIBRARIES) $(FLAGS) udp_server.c -o $(APP_NAME) 
	gcc udp_client.c -o udp_client

clean:
	rm -rf udp_client udp_server
