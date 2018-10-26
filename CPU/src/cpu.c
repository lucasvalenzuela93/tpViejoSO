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

void enviarDtbSAFA(DTB* dtb,char* tipo){
	pthread_mutex_lock(&mutexDtb);
	enviarHeader(socketSAFA, tipo, SAFA_BLOQUEAR_CPU);
	enviarDtb(socketSAFA, dtb);
	pthread_mutex_unlock(&mutexDtb);
}

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

	pSockets->socketDam = socketDam;
	pSockets->socketFm9 = socketFunesMemory;
	pSockets->socketSafa = socketSAFA;

	// ESPERO A RECIBIR EL DTB DE SAFA
	recibirMensajes();


	finalizarVariables();
	puts("Finalizo CPU");
	return EXIT_SUCCESS;
}

int decidirAccionParseo(DTB* dtbo, int pars){
	switch(pars){
		case 1:{
			dtbo->programCounter ++;
			dtbo->rafaga --;
			return 1;
		}
		case 2:{
			dtbo->programCounter ++;
			dtbo->rafaga --;
			sleep(1);
			break;
		}
		case 3:{
			dtbo->programCounter ++;
			dtbo->rafaga --;
			sleep(1);
			return 4;
		}
		default: {
//			free(linea);
//			return 3;
			break;
		}
	}
	return -1;
}


int parsearArchivo(char *path,parserSockets *parser, DTB *dtbo){
	/*
	 * 	RESUESTAS:
	 * 		-1 - error
	 * 		 1 - bloquear
	 * 		 2 - desalojar
	 * 		 3 - exit
	 * 		 4 - esperar respuesta
	 */
	char* pathF = string_from_format("/home/utnso/workspace/tp-2018-2c-keAprobo/FileSystem/montaje/%s",path);
	FILE *file = fopen(pathF, "r");
	char* linea = string_new();
	if(file == NULL){
		free(linea);
		return -1;
	}
	int i = 0;
	int pc = 0;
	char c;
	// TODO: recibir el tamaño de linea del FM9
	while(i < max_linea){
		c = (char) fgetc(file);
//		if(dtbo->rafaga == 0){
//			free(linea);
//			return 2;
//		}
		if(c == EOF){
			log_info(logger,"Fin de archivo -- %s\n", path);
			free(linea);
			return 3;
		}

		if(c == '\n'){
			if(dtbo->rafaga == 0){
				free(linea);
				return 2;
			}
			if(strlen(linea) == 0){
				log_info(logger,"Fin de archivo -- %s\n", path);
				free(linea);
				return 3;
			}
			if(dtbo->programCounter == pc){
				printf("ProgramCounter de %d -- %d\n", dtbo->idGdt, dtbo->programCounter);
				string_append_with_format(&linea,"%c", '\0');
				int pars = decidirAccionParseo(dtbo, parsearLinea(linea, parser, dtbo));
				if(pars != -1){
					return pars;
				}
				sleep(1);
			}else{
				pc ++;
			}
			linea = string_new();
		}else if(c != EOF){
			string_append_with_format(&linea,"%c", c);
		}else if(strlen(linea) > 0){
			printf("ProgramCounter de %d -- %d\n", dtbo->idGdt, dtbo->programCounter + 1);
			int pars = decidirAccionParseo(dtbo, parsearLinea(linea, parser, dtbo));
			if(pars != -1){
				return pars;
			}
		}else{
			log_info(logger,"Fin de archivo -- %s\n", path);
			free(linea);
			return 3;
		}
	}
	free(linea);
	return 1;
}

void decidirAccion(DTB* dtb, int res){
	switch(res){
		case 1:{
			log_info(logger, "DTB BLOQUEADO -- Id: %d", dtb->idGdt);
			enviarDtbSAFA(dtb, "");
			break;
		}
		case 2:{
			log_info(logger, "DTB DESALOJAR -- Id: %d", dtb->idGdt);
			enviarDtbSAFA(dtb, "desalojar");
			break;
		}
		case 3:{
			log_info(logger, "DTB EXIT -- Id: %d", dtb->idGdt);
			enviarDtbSAFA(dtb, "exit");
			break;
		}
		case 4:{
			// esperar respuesta
			break;
		}
	}
}

void ejecutarNormal(DTB* dtb){
	char* path1 = string_new();
	path1 = string_duplicate("test.txt");
	int res = parsearArchivo(path1, pSockets, dtb);
	decidirAccion(dtb, res);
	free(path1);
}

void recibirMensajes(){
	DTB* dtb;

	ContentHeader *header;
	while(true){
		header = recibirHeader(socketSAFA);
		switch(header->id){
			case ENVIAR_DTB:{
				dtb = recibirDtb(socketSAFA);
				if(dtb->flagInicio == 0){
					// ES EL DTB DUMMY
					enviarHeader(socketDam,"",CPU_PEDIR_ARCHIVO);
					enviarHeader(socketDam,dtb->pathScript,SAFA_ID_GDT_DEL_CPU);
					enviarMensaje(socketDam, dtb->pathScript);
					enviarHeader(socketDam, "", dtb->idGdt);
					enviarDtbSAFA(dtb, "");
				}else {
					// EJECUTO NORMAL
					ejecutarNormal(dtb);
				}
				break;
			}
			case WAIT_ESPERAR:{
				enviarDtbSAFA(dtb, "");
				puts("Wait Esperar");
				break;
			}
			case WAIT_OK:{
				puts("WAIT OK");
				ejecutarNormal(dtb);
				break;
			}
			case SIGNAL_OK:{
				puts("SIGNAL OK");
				ejecutarNormal(dtb);
				break;
			}
			default:{
				printf("\nId del Header: %d\n", header->id);
				break;
			}
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

	pthread_mutex_init(&mutexDtb, NULL);

	pSockets = (parserSockets*) malloc(sizeof(parserSockets));

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
