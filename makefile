CC=gcc
CFLAGS=-I.

tuan4make: server/server.c client/client.c
	$(CC) server/server.c -o server/server -pthread
	$(CC) client/client.c -o client/client
