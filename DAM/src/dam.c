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

	socketSafa = clienteConectarComponente("DAM","S-AFA", PUERTO, IP);

	return EXIT_SUCCESS;
}
