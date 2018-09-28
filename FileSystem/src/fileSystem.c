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
	iniciarVariables();

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP_ESCUCHA, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FILE_SYSTEM", "DAM");

	finalizarVariables();
	puts("Finalizo File System...");
	return EXIT_SUCCESS;
}

void iniciarVariables(){
	config = config_create("./config/config.txt");
	if(config == NULL){
		puts("Error al levantar configuraciones...");
		finalizarVariables();
		exit(1);
	}

	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");

	puts("Variables inicadas...");
}

void finalizarVariables(){
	shutdown(socketEscucha,2);
	shutdown(socketDam,2);
}
