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
	iniciarVariables();

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FUNES_MEMORY", "DAM");
	socketCpu = servidorConectarComponente(socketEscucha, "FUNES_MEMORY", "CPU");

	finalizarVariables();
	puts("Finalizo Funes Memory...");
	return EXIT_SUCCESS;
}

void iniciarVariables(){
	config = config_create("./config/config.txt");
	if(config == NULL){
		puts("Error al leer configuraciones...");
		finalizarVariables();
		exit(1);
	}
	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
}

void finalizarVariables(){
	shutdown(socketEscucha,2);
	shutdown(socketDam,2);
	shutdown(socketCpu,2);
}
