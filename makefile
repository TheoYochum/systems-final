compile: bogosort.c bogosort.h
	gcc -c bogosort.c
	gcc -o bogo bogosort.o

single: bogo
	./bogo single

local: bogo
	./bogo local

server: bogo
	./bogo server

client: bogo
	./bogo client
