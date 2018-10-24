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
	int i = printf("El tamaño de linea es:\n");
	iniciarVariables();

	socketEscucha = socketServidor(puertoEscucha, ipEscucha, 50);
//
	socketDam = servidorConectarComponente(socketEscucha, "FUNES_MEMORY", "DAM");

	char *max_tam_linea = string_itoa(max_linea);
	printf("El tamaño de linea es: %s\n", max_tam_linea);
	enviarHeader(socketDam, max_tam_linea, FM9_ENVIAR_MAX_TAM_LINEA);
	enviarMensaje(socketDam, max_tam_linea);



	recibirGDT();

	while(1){

	}

//	esperarConexiones();
//
//
//	finalizarVariables();
//	puts("Finalizo Funes Memory...");
	return EXIT_SUCCESS;
}

void esperarConexiones(){
	fd_set descLectura;
	struct timeval timeout;
	int numeroClientes = 0;
	int socketCpu[100];
	int nextCpuId = 1;

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
			socketCpu[numeroClientes] = servidorConectarComponente(socketEscucha, "FUNES_MEMORY", "CPU");
			numeroClientes++;
			CPU_struct *cpu = (CPU_struct*) malloc(sizeof(CPU_struct));
			cpu->id = nextCpuId;
			cpu->socket = socketCpu[numeroClientes - 1];
			printf("Se conecto CPU con id: %d \n", cpu->id);
			list_add(listaCpu, (void*) cpu);
			free(cpu);
			nextCpuId ++;

		}
	}
}

void iniciarVariables(){
	config = config_create("/home/utnso/workspace/tp-2018-2c-keAprobo/FunesMemory/config/config.txt");
	if(config == NULL){
		puts("Error al leer configuraciones...");
		finalizarVariables();
		exit(1);
	}
	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	max_linea = config_get_int_value(config, "MAX_LINEA");
	listaCpu = list_create();
}

void recibirGDT(int idGDT){ //recibir y guardar idGDT de DTB desde DAM
		ContentHeader *header = recibirHeader(socketDam);
		header = recibirHeader(socketDam);
		puts("Recibo idGDT de proceso DAM");
		idGDT = header->id;
		free(header);
		guardarGDT(idGDT);
}

void guardarGDT(int idGDT){

	lista_idGDTs = list_create();
	list_add(lista_idGDTs, (void*)idGDT);
}


void finalizarVariables(){
	shutdown(socketEscucha,2);
	shutdown(socketDam,2);
	shutdown(socketCpu,2);
}
