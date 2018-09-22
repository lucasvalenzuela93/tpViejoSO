/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "fileSystem.h"

int main(void) {
	puts("Iniciando File System...");

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP_ESCUCHA, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FILE_SYSTEM", "DAM");

	return EXIT_SUCCESS;
}