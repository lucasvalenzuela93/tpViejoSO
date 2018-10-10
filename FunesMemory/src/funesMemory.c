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

	esperarConexiones();

	finalizarVariables();
	puts("Finalizo Funes Memory...");
	return EXIT_SUCCESS;
}

void esperarConexiones(){
	fd_set descLectura;
	struct timeval timeout;
	int numeroClientes = 0;
	int socketCpu[100];
	int nextCpuId = 1;
	socketEscucha = socketServidor(puertoEscucha, ipEscucha, 50);

	while(true){
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		// inicio descriptoresLectura
		FD_ZERO(&descLectura);
		// se añade para select() el socket servidor
		FD_SET(socketEscucha, &descLectura);
		//añado los CPU ya conectados
		for(int i = 0; i< numeroClientes; i++){
			FD_SET(socketCpu[i], &descLectura);
		}
		// escucho a nuevas conexiones
		select(100, &descLectura, NULL,NULL,&timeout);

		if(FD_ISSET(socketEscucha, &descLectura)){
			socketCpu[numeroClientes] = servidorConectarComponente(socketEscucha, "FM9", "CPU");
			numeroClientes++;
			CPU_struct *cpu = malloc(sizeof(CPU_struct));
			cpu->id = nextCpuId;
			cpu->socket = socketCpu[numeroClientes - 1];
			nextCpuId ++;
			printf("Se conecto CPU con id: %d \n", cpu->id);
			free(cpu);


		}
	}
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
