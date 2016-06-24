#
# Archivo: Makefile
#
# Nombres:
#	Alejandra Cordero / Carnet: 12-10645
#	Ricardo Mena  / Carnet: 12-10872
#
# Ultima modificacion: 09/05/2015
#
#

all:
	gcc -Wall -g cliente.c -o sem_cli
	gcc -Wall -g servidor.c lib_servidor.c -o sem_srv

sem_srv.o: servidor.c lib_servidor.h
	gcc -c servidor.c

clean:
	rm sem_cli
	rm sem_srv