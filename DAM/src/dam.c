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



	socketSafa = clienteConectarComponente("DAM","S-AFA", PUERTO_SAFA, IP_SAFA);

	socketFileSystem = clienteConectarComponente("DAM", "FILE_SYSTEM", PUERTO_FILE_SYSTEM, IP_SAFA);

	// DEBE PRIMERO CONECTARSE FUNES MEMORY
	// ANTES DE ESCUCHAR CONEXIONES DEL CPU

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP_SAFA, 50);

	socketCpu = servidorConectarComponente(socketEscucha, "DAM", "CPU");

	return EXIT_SUCCESS;
}
