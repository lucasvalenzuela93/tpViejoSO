/*
 * sockets.h
 *
 *  Created on: 22 abr. 2018
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_
#define DEBUG_SOCKET 0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <arpa/inet.h>

int MAX_CONEX;
enum PROTOCOLO {
	// MENSAJES PARA DECIR QUIEN SOY
	CPU = 1,
	SAFA = 2,
	DAM = 3,
	FM9 = 4,

	// MENSAJES PARA AVISAR AL CPU
	CPU_MAXIMAS_CONEXIONES_ALCANZADAS = 5,
	CPU_BLOQUEAR = 6,
	CPU_CREADO_OK = 7,
	CPU_CAIDO = 8,


	//MENSAJES ENTRE CPU A DAM
	CPU_PEDIR_ARCHIVO = 9,
	DAM_ENVIAR_ARCHIVO = 10,

	// MENSAJES PARA AVISAR AL SAFA
	SAFA_MEMORIA_CARGADA = 11,
	SAFA_DATOS_CPU = 12,
	SAFA_MOVER_CPU_EXIT = 13,

	// MENSAJES FM9 Y MDJ(FILES SYSTEM)
	DAM_GET_PATH_MDJ = 14,
	DAM_SEND_PATH_FM9 = 15,

	// MENSAJES DEL FM9
	FM9_PATH_STORE_OK = 16,
	FM9_PATH_STORE_ERROR = 17,
	FM9_PATH_GET_OK = 18,
	FM9_PATH_GET_ERROR = 19,
	FM9_ESPACION_INSUFICIENTE = 26,


	// MENSAJES DEL MDJ(FILE SYSTEM)
	MDJ_PATH_STORE_OK = 20,
	MDJ_PATH_STORE_ERROR = 21,
	MDJ_PATH_GET_OK = 22,
	MDJ_PATH_GET_ERROR = 22,
	MDJ_ARCHIVO_INVALIDO = 23,
	MDJ_CREACION_ARCHIVO_OK = 24,
	MDJ_ERROR_CREACION_ARCHIVO = 25,


//
//	// MENSAJES PARA AVISAR AL ESI POR UN SET
//	COORDINADOR_ESI_ERROR_TAMANIO_CLAVE = 9,
//	COORDINADOR_ESI_ERROR_CLAVE_NO_IDENTIFICADA = 10,
//	COORDINADOR_ESI_ERROR_CLAVE_NO_TOMADA = 11,
//
//
//
//	// MENSAJE PARA AVISAR INSTANCIA DESCONECTADA
//	INSTANCIA_COORDINADOR_DESCONECTADA = 12,
//
//	// MENSAJES PARA RESPONDER LA INSTANCIA
//	INSTANCIA_SENTENCIA_OK_SET = 13,
//	INSTANCIA_SENTENCIA_OK_STORE = 14,
//	INSTANCIA_CLAVE_NO_IDENTIFICADA = 15,
//	INSTANCIA_ERROR = 16,
//
//	// COMPACTACION
//	COMPACTAR = 18,
//
//	// COMANDOS DEL PLANIFICADOR AL COORDINADOR
//	COMANDO_KILL = 19,
//	COMANDO_STATUS = 20,
//	// COMANDOS DE LA INSTANCIA AL COORDINADOR
//	COMANDO_STATUS_VALOR_CLAVE_OK = 21,
//	COMANDO_STATUS_VALOR_CLAVE_NULL = 22,
//	BLOQUEAR_CLAVE_MANUAL = 23,
//	DESBLOQUEAR_CLAVE_MANUAL = 24
};

//estructuras
typedef struct {
  int id;
  int largo;
} __attribute__((packed)) ContentHeader;

//prototipos
int conectarClienteA(int, char*);
int enviarInformacion(int, void*, int*);
int socketServidor(int, char*, int);
int enviarHeader(int, char*, int);
int enviarMensaje(int, char*);
void recibirMensaje(int, int, char**);
ContentHeader * recibirHeader(int);
int servidorConectarComponente(int, char*, char*);
int clienteConectarComponente(char*, char*, int, char*);

#endif /* SOCKETS_H_ */
