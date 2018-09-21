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
	inicializarVariables();

	socketEscucha = socketServidor(PUERTO, IP, 50);
	socket_cpu = servidorConectarComponente(socketEscucha, "S-AFA", "CPU");

	return EXIT_SUCCESS;
}

void inicializarVariables(){
	logger = log_create("log.txt","S-AFA",true, LOG_LEVEL_INFO);

	puts("Variables inicializadas...");

}
