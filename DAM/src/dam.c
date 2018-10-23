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

	pthread_t hiloMDJ;
	if(pthread_create(&hiloMDJ, NULL, recibirYEnviarMDJ, NULL)){
		puts("Error al crear hilo de esucha mdj");
		finalizarVariables();
		exit(1);
	}

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
			case DAM_CREAR:{
				char* path = malloc(header->largo);
				int idDtb, cantLineas;
				recibirMensaje(cpu->socket, header->largo, &path);
				printf("PATH: %s\n", path);
				printf("Largo Header: %d\n", header->largo);
				// RECIBO EL ID DEL DTB
				header = recibirHeader(cpu->socket);
				idDtb = header->id;
				// LUEGO RECIBO LA CANTIDAD DE LINEAS
				header = recibirHeader(cpu->socket);
				cantLineas = header->id;
				// DEBO ENVIAR AL MDJ LOS DATOS PARA QUE CREE EL ARCHIVO
				pthread_mutex_lock(&mutexEnviarMDJ);
				enviarHeader(socketFileSystem,path,DAM_CREAR);
				enviarHeader(socketFileSystem, path, idDtb);
				enviarHeader(socketFileSystem, path, cantLineas);
				enviarMensaje(socketFileSystem, path);
				pthread_mutex_unlock(&mutexEnviarMDJ);
				puts("DAM Crear");

//				// TEST PARA Q SAFA MUEVA A READY
//				sleep(2);
//				enviarHeader(socketSafa, "", SAFA_DESBLOQUEAR_CPU);
//				enviarHeader(socketSafa,"", header->id);
				free(path);
				break;
			}
			case DAM_BORRAR:{
				puts("DAM borrar");
				int idDtb;
				// RECIBO EL ID DEL DTB
				header = recibirHeader(cpu->socket);
				idDtb = header->id;
				char* path = malloc(header->largo);
				// RECIBO EL PATH DEL CPU
				recibirMensaje(cpu->socket, header->largo, &path);
				// DEBO ENVIAR AL MDJ LOS DATOS PARA QUE BORRE EL ARCHIVO
				pthread_mutex_lock(&mutexEnviarMDJ);
				enviarHeader(socketFileSystem,path,DAM_BORRAR);
				enviarHeader(socketFileSystem, path, idDtb);
				enviarMensaje(socketFileSystem, path);
				pthread_mutex_unlock(&mutexEnviarMDJ);
				free(path);
				break;
			}
			case DAM_FLUSH:{

				break;
			}
		}
	}
	free(header);
}

void recibirYEnviarMDJ(){
	puts("Escucho mensajes MDJ");
	ContentHeader *header;
	while(true){
		header = recibirHeader(socketFileSystem);
		printf("Recibi header MDJ -- id: %d\n", header->id);
		int headId = header->id;
		if(headId == MDJ_CREACION_ARCHIVO_OK || headId == MDJ_BORRAR_OK || headId == MDJ_FLUSH_OK){
			puts("Desbloquear DTB");
			header = recibirHeader(socketFileSystem);
			// REENVIO EL MENSAJE AL SAFA AVISANDOLE QUE DESBLOQUEE A EL GDT
			printf("DTB a desbloquear: %d\n", header->id);
			enviarHeader(socketSafa, "", SAFA_DESBLOQUEAR_CPU);
			enviarHeader(socketSafa,"", header->id);
			continue;
		}
		switch(header->id){
			case MDJ_CREACION_ARCHIVO_OK:{

				break;
			}
			case MDJ_BORRAR_OK:{
				header = recibirHeader(socketFileSystem);

			}
		}
	}
	free(header);
}

void iniciarVariables(){
	config = config_create(PATH_CONFIG);
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

	pthread_mutex_init(&mutexEnviarMDJ, NULL);

	puts("Variables iniciadas...");
}

void finalizarVariables(){
	shutdown(socketSafa,2);
	shutdown(socketFileSystem,2);
	shutdown(socketFunesMemory,2);
	shutdown(socketEscucha,2);
}
