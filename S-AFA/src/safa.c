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


	socket_dam = servidorConectarComponente(socketEscucha, "S-AFA", "DAM");
	// CREO THREAD PARA QUE ESPERE MENSAJES DEL DAM
	pthread_t hiloDam;
	char* componente = malloc(4);
	strcpy(componente, "DAM");
	if(pthread_create(&hiloDam,NULL, esperarMensajesDAM, &socket_dam ) != 0){
		// ERROR AL CREAR EL HILO
		puts("Error al crear hilo que escucha al DAM...");
	}

	socket_cpu = servidorConectarComponente(socketEscucha, "S-AFA", "CPU");
	while(true);



	return EXIT_SUCCESS;
}

void inicializarVariables(){
	logger = log_create("log.txt","S-AFA",true, LOG_LEVEL_INFO);

	puts("Variables inicializadas...");
}

void* esperarMensajesDAM(void* socket){
	int socket_dam = (int) socket;
	printf("%d", socket_dam);

	return NULL;
}

