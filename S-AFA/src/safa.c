/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "safa.h"

int main(void) {
	crearLogger();

	socketEscucha = conectarComoServidor(logger, IP, PUERTO);
	int nuevoSocketEscuchado = escucharCliente(logger, socketEscucha);
	list_add(socketsEscuchando, &nuevoSocketEscuchado);

	return EXIT_SUCCESS;
}

void crearLogger(){
	logger = log_create("log.txt","S-AFA",true, LOG_LEVEL_INFO);
	puts("Log iniciado");
}

void inicializarVariables(){
	socketsEscuchando = list_create();
}
