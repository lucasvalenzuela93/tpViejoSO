/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "funesMemory.h"

int main(void) {
	puts("Iniciando Funes Memory...");

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FUNES_MEMORY", "DAM");

	return EXIT_SUCCESS;
}
