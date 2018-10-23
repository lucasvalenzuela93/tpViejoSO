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
#include <commons/collections/list.h>

int MAX_CONEX;

#define ID "ID"
#define RAFAGA "RAFAGA"
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
	CPU_EJECUTAR = 29,
	CPU_FRENAR_EJECUCION = 30,
	CPU_RECIBIR_DTB = 31,


	//MENSAJES ENTRE CPU A DAM
	CPU_PEDIR_ARCHIVO = 9,
	DAM_ENVIAR_ARCHIVO = 10,

	// MENSAJES PARA AVISAR AL SAFA
	SAFA_MEMORIA_CARGADA = 11,
	SAFA_DATOS_CPU = 12,
	SAFA_MOVER_CPU_EXIT = 13,
	SAFA_BLOQUEAR_CPU = 26,
	SAFA_DESBLOQUEAR_CPU = 27,
	SAFA_MATAR_CPU = 28,
	SAFA_ID_GDT_DEL_CPU = 34,
	SAFA_PC_DEL_CPU = 35,

	// MENSAJES FM9 Y MDJ(FILES SYSTEM)
	DAM_GET_PATH_MDJ = 14,
	DAM_SEND_PATH_FM9 = 15,
	DAM_ABRIR = 38,

	// MENSAJES DEL FM9
	FM9_PATH_STORE_OK = 16,
	FM9_PATH_STORE_ERROR = 17,
	FM9_PATH_GET_OK = 18,
	FM9_PATH_GET_ERROR = 19,
	FM9_ESPACION_INSUFICIENTE = 26,
	FM9_ENVIAR_MAX_TAM_LINEA = 200,


	// MENSAJES DEL MDJ(FILE SYSTEM)
	MDJ_PATH_STORE_OK = 20,
	MDJ_PATH_STORE_ERROR = 21,
	MDJ_PATH_GET_OK = 22,
	MDJ_PATH_GET_ERROR = 37,
	MDJ_CHECK_PATH = 28,
	MDJ_ARCHIVO_INVALIDO = 23,
	MDJ_CREACION_ARCHIVO_OK = 24,
	MDJ_ERROR_CREACION_ARCHIVO = 25,

	// MENSAJES DE RESPUESTA

	RTA_OK = 32,
	RTA_ERROR = 33,
	NUEVA_CONEXION = 35,
	ENVIAR_DTB = 36,
};

//estructuras
typedef struct {
  int id;
  int largo;
} __attribute__((packed)) ContentHeader;

typedef struct DTB {
	int idGdt;
	char* pathScript;
	int programCounter;
	int flagInicio;
	int socket;
} __attribute__((packed)) DTB;

typedef struct DTB_aux {
	int idGdt;
	int programCounter;
	int flagInicio;
	int socket;
	int tamanioPath;
} __attribute__((packed)) DTB_aux;

//prototipos
int conectarClienteA(int, char*);
int enviarInformacion(int, void*, int*);
int socketServidor(int, char*, int);
int enviarDtb(int, DTB*);
DTB* recibirDtb(int);
int enviarHeader(int, char*, int);
int enviarMensaje(int, char*);
int enviarHeaderYMensaje(int,char*,int);
void recibirMensaje(int, int, char**);
ContentHeader * recibirHeader(int);
int servidorConectarComponente(int, char*, char*);
int clienteConectarComponente(char*, char*, int, char*);

#endif /* SOCKETS_H_ */
