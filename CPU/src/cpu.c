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
	inciarVariables();

	socketSAFA = clienteConectarComponente("CPU","S-AFA", puertoSafa, ipSafa);
	socketDam = clienteConectarComponente("CPU","DAM", puertoDam, ipDam);
	socketFunesMemory = clienteConectarComponente("CPU","FUNES_MEMORY", puertoFunesMemory, ipFunesMemory);

	finalizarVariables();
	puts("Finalizo CPU");
	return EXIT_SUCCESS;
}


void inciarVariables(){
	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_INFO);
	config = config_create("./config/config.txt");
	if(config == NULL){
		puts("Error al leer configuraciones...");
		finalizarVariables();
		exit(1);
	}
	ipSafa = config_get_string_value(config, "IP_SAFA");
	ipDam = config_get_string_value(config, "IP_DAM");
	ipFunesMemory = config_get_string_value(config, "IP_FUNES_MEMORY");

	puertoSafa = config_get_int_value(config, "PUERTO_SAFA");
	puertoDam = config_get_int_value(config, "PUERTO_DAM");
	puertoFunesMemory = config_get_int_value(config, "PUERTO_FUNES_MEMORY");

	puts("Variables iniciadas...");

}
void finalizarVariables(){
	shutdown(socketSAFA,2);
	shutdown(socketDam,2);
	shutdown(socketFunesMemory,2);
}
