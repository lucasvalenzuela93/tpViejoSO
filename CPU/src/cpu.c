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
	if(recv(socketSAFA, &self, sizeof(InfoCpu), 0) < 0){
		puts("Error al recibir informacion del handshake con el S-AFA");
		close(socketSAFA);
		config_destroy(config);
		exit(1);
	}

	socketDam = clienteConectarComponente("CPU","DAM", puertoDam, ipDam);
	socketFunesMemory = clienteConectarComponente("CPU","FUNES_MEMORY", puertoFunesMemory, ipFunesMemory);

	// ESPERO A RECIBIR EL DTB DE SAFA
	recibirMensajes();


	finalizarVariables();
	puts("Finalizo CPU");
	return EXIT_SUCCESS;
}

void recibirMensajes(){
	DTB* dtb;
	parserSockets *pSockets = malloc(sizeof(parserSockets));
	pSockets->socketDam = socketDam;
	pSockets->socketFm9 = socketFunesMemory;
	pSockets->socketSafa = socketSAFA;
	while(true){
		dtb = recibirDtb(socketSAFA);
		if(dtb->flagInicio == 0){
			// ES EL DTB DUMMY
			puts("DTB Dummy");
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
			puts(" DTB Normal");
			int res = parsearLinea("borrar equipos/River.txt", pSockets, dtb);
			printf("resultado: %d\n", res);
//			sleep(3);
//			res = parsearLinea("borrar equipos/River.txt", pSockets, dtb);
//			printf("resultado: %d", res);
		}
		sleep(1);
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
