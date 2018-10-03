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


	socketSafa = clienteConectarComponente("DAM","S-AFA", puertoSafa, ipSafa);

	socketFileSystem = clienteConectarComponente("DAM", "FILE_SYSTEM", puertoFileSystem, ipFileSystem);

	socketFunesMemory = clienteConectarComponente("DAM", "FUNES_MEMORY", puertoFunesMemory,ipFunesMemory);

	esperarConexiones();




	finalizarVariables();
	puts("Finalizo DAM...");
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
			socketCpu[numeroClientes] = servidorConectarComponente(socketEscucha, "DAM", "CPU");
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
	config = config_create("../config/config.txt");
	if(config == NULL){
		puts("Error al abrir archivo de configuracion...");
		finalizarVariables();
		exit(1);
	}
	ipEscucha = (char*)config_get_string_value(config,"IP_ESCUCHA");
	ipSafa = (char*) config_get_string_value(config, "IP_SAFA");
	ipFileSystem = (char*) config_get_string_value(config, "IP_FILE_SYSTEM");
	ipFunesMemory = (char*) config_get_string_value(config, "IP_FUNES_MEMORY");

	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	puertoSafa = config_get_int_value(config, "PUERTO_SAFA");
	puertoFileSystem = config_get_int_value(config, "PUERTO_FILE_SYSTEM");
	puertoFunesMemory = config_get_int_value(config, "PUERTO_FUNES_MEMORY");

	listaCpu = list_create();
	puts("Variables iniciadas...");
}

void finalizarVariables(){
	shutdown(socketSafa,2);
	shutdown(socketFileSystem,2);
	shutdown(socketFunesMemory,2);
	shutdown(socketEscucha,2);
	shutdown(socketCpu,2);
}
