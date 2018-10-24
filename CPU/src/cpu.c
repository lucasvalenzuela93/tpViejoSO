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
	// RECIBO EL HEADER DEL SAFA CON MI ID Y LA RAFAGA
	if(recv(socketSAFA, self, sizeof(InfoCpu), 0) < 0){
		puts("Error al recibir informacion del handshake con el S-AFA");
		close(socketSAFA);
		config_destroy(config);
		exit(1);
	}

	socketDam = clienteConectarComponente("CPU","DAM", puertoDam, ipDam);

	socketFunesMemory = clienteConectarComponente("CPU","FUNES_MEMORY", puertoFunesMemory, ipFunesMemory);
	max_linea = recibirTamMaxLinea();
	printf("Max_linea: %d\n", max_linea);

	// ESPERO A RECIBIR EL DTB DE SAFA
	recibirMensajes();


	finalizarVariables();
	puts("Finalizo CPU");
	return EXIT_SUCCESS;
}

int parsearArchivo(char *path,parserSockets *parser, DTB *dtbo){
	int tamanioLinea = 128;
	char* pathF = string_from_format("/home/utnso/workspace/tp-2018-2c-keAprobo/FileSystem/montaje/%s",path);
	FILE *file = fopen(pathF, "r");
	char* linea = string_new();
	if(file == NULL){
		free(linea);
		return -1;
	}
	int i = 0;
	char c;
	// TODO: recibir el tamaño de linea del FM9
	while(i < tamanioLinea){
		c = (char) fgetc(file);

		if(c == '\n'){
			if(strlen(linea) == 0){
				free(linea);
				return -1;
			}
			string_append_with_format(&linea,"%c", '\0');
			printf("ProgramCounter de %d -- %d\n", dtbo->idGdt, dtbo->programCounter + 1);
			if(parsearLinea(linea, parser, dtbo) == 1){
				dtbo->programCounter ++;
//				if(dtbo->programCounter % self->rafaga == 0 && dtbo->programCounter != 0){
//					log_info(logger, "DESALOJO -- Id: %d\n", dtbo->idGdt);
//					enviarHeader(socketSAFA,"desalojar", SAFA_BLOQUEAR_CPU);
//					enviarDtb(socketSAFA, dtbo);
//					break;
//				}
			}
			linea = string_new();

			sleep(1);
		}else if(c != EOF){
			string_append_with_format(&linea,"%c", c);
		}else if(strlen(linea) > 0){
			printf("ProgramCounter de %d -- %d\n", dtbo->idGdt, dtbo->programCounter + 1);
			if(parsearLinea(linea, parser, dtbo) == 1){
				dtbo->programCounter ++;
			}
			log_info(logger, "Fin de archivo");
			enviarHeader(socketSAFA, "exit", SAFA_BLOQUEAR_CPU);
			enviarDtb(socketSAFA, dtbo);
			break;
		}else{
			log_info(logger, "Fin de archivo");
			enviarHeader(socketSAFA, "exit", SAFA_BLOQUEAR_CPU);
			enviarDtb(socketSAFA, dtbo);
			break;
		}
	}
//	if(dtbo->programCounter == self->rafaga){
//		printf("DESALOJO DTB: %d\n\tProgram Counter: %d\n", dtbo->idGdt, dtbo->programCounter);
//		free(linea);
//		return 2;
//	}
	free(linea);
	return 1;
}

void recibirMensajes(){
	DTB* dtb;
	parserSockets *pSockets = (parserSockets*) malloc(sizeof(parserSockets));
	pSockets->socketDam = socketDam;
	pSockets->socketFm9 = socketFunesMemory;
	pSockets->socketSafa = socketSAFA;
	ContentHeader *header;
	while(true){
		header = recibirHeader(socketSAFA);
		switch(header->id){
			case ENVIAR_DTB:{
				dtb = recibirDtb(socketSAFA);
				if(dtb->flagInicio == 0){
					// ES EL DTB DUMMY
					// LE AVISO AL DMA QUE BUSQUE EL ESCRIPTORIO
					enviarHeader(socketDam,"",CPU_PEDIR_ARCHIVO);
					// LE ENVIO EL ID DEL GDT Y EL PATH
					enviarHeader(socketDam,dtb->pathScript,SAFA_ID_GDT_DEL_CPU);
					enviarMensaje(socketDam, dtb->pathScript);
					enviarHeader(socketDam, "", dtb->idGdt);
					// LE DIGO AL SAFA QUE ME BLOQUEE
					enviarHeader(socketSAFA,"", SAFA_BLOQUEAR_CPU);
					enviarDtb(socketSAFA, dtb);
					puts("DTB enviado...");
				}else {
					// EJECUTO NORMAL
					char* path1 = string_new();
					path1 = string_duplicate("test.txt");
					parsearArchivo(path1, pSockets, dtb);
					free(path1);
				}
				break;
			}
			case WAIT_ESPERAR:{
				enviarHeader(socketSAFA,"", SAFA_BLOQUEAR_CPU);
				enviarDtb(socketSAFA, dtb);
				puts("Wait Esperar");
				break;
			}
			default: break;
		}


		puts("complete cliclo");
	}
	free(dtb);
	puts("finaliza cpu");

}

void inciarVariables(){
	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_INFO);
	config = config_create(PATH_CONFIG);
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

	self = (InfoCpu*) malloc(sizeof(InfoCpu));

	puts("Variables iniciadas...");

}
void finalizarVariables(){
	shutdown(socketSAFA,2);
	shutdown(socketDam,2);
	shutdown(socketFunesMemory,2);
	config_destroy(config);
}

int recibirTamMaxLinea(){
	char *max_tam_linea = string_new();
	ContentHeader * header;
	header = recibirHeader(socketFunesMemory);
	if(header->id == FM9_ENVIAR_MAX_TAM_LINEA)
		recibirMensaje(socketFunesMemory, header->largo, &max_tam_linea);
	int max_tam_linea_int = atoi(max_tam_linea);
	return max_tam_linea_int;
}
