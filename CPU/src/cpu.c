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

	crearLogger();
	socketSAFA = conectarComoCliente(logger, IP, PUERTO);

	return EXIT_SUCCESS;
}

void crearLogger(){
	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_INFO);
	puts("Log iniciado");
}
