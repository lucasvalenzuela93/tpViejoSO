/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "dam.h"

int main(void) {
	puts("Iniciando DAM...");
	iniciarVariables();


	socketSafa = clienteConectarComponente("DAM","S-AFA", PUERTO_SAFA, IP_SAFA);

	socketFileSystem = clienteConectarComponente("DAM", "FILE_SYSTEM", PUERTO_FILE_SYSTEM, IP_SAFA);

	socketFunesMemory = clienteConectarComponente("DAM", "FUNES_MEMORY", PUERTO_FUNES_MEMORY,IP_SAFA);

	// DEBE PRIMERO CONECTARSE FUNES MEMORY
	// ANTES DE ESCUCHAR CONEXIONES DEL CPU

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP_SAFA, 50);

	socketCpu = servidorConectarComponente(socketEscucha, "DAM", "CPU");


	finalizarVariables();
	puts("Finalizo DAM...");
	return EXIT_SUCCESS;
}
void iniciarVariables(){
	config = config_create("../config/config.txt");
	if(config == NULL){
		puts("Error al abrir archivo de configuracion...");
		exit(1);
	}
	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	ipSafa = config_get_string_value(config, "IP_SAFA");
	ipFileSystem = config_get_string_value(config, "IP_FILE_SYSTEM");
	ipFunesMemory = config_get_string_value(config, "IP_FUNES_MEMORY");

	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	puertoSafa = config_get_int_value(config, "PUERTO_SAFA");
	puertoFileSystem = config_get_int_value(config, "PUERTO_FILE_SYSTEM");
	puertoFunesMemory = config_get_int_value(config, "PUERTO_FUNES_MEMORY");

	puts("Variables iniciadas...");
}

void finalizarVariables(){
	shutdown(socketSafa,2);
	shutdown(socketFileSystem,2);
	shutdown(socketFunesMemory,2);
	shutdown(socketEscucha,2);
	shutdown(socketCpu,2);
}
