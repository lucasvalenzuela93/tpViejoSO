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

void enviarAFileSystem(char* mensaje, int idDtb, int id){
	// WRAPER DE ENVIO DE INFO A FILE SYSTEM PARA HACERLO SINCORNIZADO
//	pthread_mutex_lock(&mutexEnviarMDJ);
	if(mensaje && idDtb){
		enviarHeader(socketFileSystem,"",id);
		enviarHeaderYMensaje(socketFileSystem, mensaje, idDtb);
	}else if(!mensaje){
		enviarHeader(socketFileSystem,"",id);
	}else {
		enviarHeaderYMensaje(socketFileSystem,mensaje,id);
	}
//	pthread_mutex_unlock(&mutexEnviarMDJ);
}

int main(void) {
	puts("Iniciando DAM...");
	iniciarVariables();


//	socketSafa = clienteConectarComponente("DAM","S-AFA", puertoSafa, ipSafa);
//
//	socketFileSystem = clienteConectarComponente("DAM", "FILE_SYSTEM", puertoFileSystem, ipFileSystem);

	socketFunesMemory = clienteConectarComponente("DAM", "FUNES_MEMORY", puertoFunesMemory,ipFunesMemory);

	char *max_tam_linea = string_new();
	printf("Max tam linea vale: %s\n", max_tam_linea);
	ContentHeader * header;
	header = recibirHeader(socketFunesMemory);
	if(header->id == FM9_ENVIAR_MAX_TAM_LINEA)
		recibirMensaje(socketFunesMemory, header->largo, &max_tam_linea);
	printf("Max_tam_linea vale: %s\n", max_tam_linea);
	int max_tam_linea_int = atoi(max_tam_linea);
	printf("Max_tam_linea vale: %d\n", max_tam_linea_int);


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
			if(pthread_create(&cpu->hilo, NULL, recibirYEnviarCPU,(void*) cpu)){
				free(cpu);
				exit(1);
			}
			nextCpuId ++;
			printf("Se conecto CPU con id: %d \n", cpu->id);
			list_add(listaCpu, (void*) cpu);
		}
	}

	for(int i = 0; i > 0; i--){
		CPU_struct* cpu = (CPU_struct*) list_remove(listaCpu, i);
		if(pthread_join(cpu->hilo, NULL) != 0){
			puta("Error al Joinear hilo de esucha CPU");
			close(cpu->socket);
			free(cpu);
			exit(1);
		}
		FD_ZERO(&descLectura);
	}
	close(socketEscucha);
}

void recibirYEnviarCPU(void* cpuVoid){
	CPU_struct *cpu = (CPU_struct*) cpuVoid;
	ContentHeader *header;
	while(true){
		header = recibirHeader(cpu->socket);
		switch(header->id){
			case CPU_PEDIR_ARCHIVO:{
				puts("PAth recibido");
				// RECIBO El LARGO DEL PATH DEL CPU
				header = recibirHeader(cpu->socket);
				char* path = malloc(header->largo);
				// RECIBO EL PATH DEL CPU
				recibirMensaje(cpu->socket, header->largo, &path);
				// RECIBO EL ID
				header = recibirHeader(cpu->socket);
				// LE AVISO AL MDJ QUE ABRA EL PATH
//				enviarAFileSystem(path,header->id,DAM_ABRIR);

				// TEST PARA Q SAFA MUEVA A READY
				sleep(2);
				enviarHeader(socketSafa, "", SAFA_DESBLOQUEAR_CPU);
				enviarHeader(socketSafa,"", header->id);
				break;
			}
		}
	}
	free(header);
}

void recibirYEnviarMDJ(){
	while(true){
		ContentHeader *header = recibirHeader(socketFileSystem);
		switch(header->id){
			case MDJ_PATH_GET_OK:{
				// DEBO AVISARLE AL SAFA QUE DESBLOQUEE A DICHO
				// ver si tengo q mandar el id del DTB y sino de donde lo saco
				puts("Desbloquear CPU");
				int idGdt;
				header = recibirHeader(socketFileSystem);
				idGdt = header->id;
				char* path = malloc(header->largo);
				recibirMensaje(socketFileSystem,header->largo,path);
				// REENVIO EL MENSAJE AL SAFA AVISANDOLE QUE DESBLOQUEE A EL GDT
				enviarHeader(socketSafa, "", SAFA_DESBLOQUEAR_CPU);
				enviarHeader(socketSafa,"", idGdt);
				puts("safa avisado del bloqueo");
				break;
			}
		}
	}
}

void iniciarVariables(){
	config = config_create("/home/utnso/workspace/tp-2018-2c-keAprobo/DAM/config/config.txt");
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
}
