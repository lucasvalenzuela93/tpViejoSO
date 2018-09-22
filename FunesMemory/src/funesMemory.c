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
	socketCpu = servidorConectarComponente(socketEscucha, "FUNES_MEMORY", "CPU");

	finalizarVariables();
	puts("Finalizo Funes Memory...");
	return EXIT_SUCCESS;
}

void finalizarVariables(){
	shutdown(socketEscucha,2);
	shutdown(socketDam,2);
	shutdown(socketCpu,2);
}
