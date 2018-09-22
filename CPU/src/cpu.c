/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "cpu.h"

int main(void) {
	puts("Iniciando CPU...");
	crearLogger();

	socketSAFA = clienteConectarComponente("CPU","S-AFA", PUERTO, IP);
	socketDam = clienteConectarComponente("CPU","DAM", PUERTO_DAM, IP);
	socketFunesMemory = clienteConectarComponente("CPU","FUNES_MEMORY", PUERTO_FUNES_MEMORY, IP);

	finalizarVariables();
	puts("Finalizo CPU");
	return EXIT_SUCCESS;
}

void crearLogger(){
	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_INFO);
	puts("Log iniciado");
}
void finalizarVariables(){
	shutdown(socketSAFA,2);
	shutdown(socketDam,2);
	shutdown(socketFunesMemory,2);
}
