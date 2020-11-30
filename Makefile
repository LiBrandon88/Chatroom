#Makefile                                                                                                                                                                                                                                     

all: client server

client: chatty_client.c
        gcc chatty_client.c -std=c99 -pthread -Wall -o client

server: chatty_server.c
        gcc chatty_server.c queue.c -std=c99 -pthread -Wall -o server

clean: rm -f client server


