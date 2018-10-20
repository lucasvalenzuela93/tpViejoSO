/*
 * funesMemory.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef SRC_FUNESMEMORY_H_
#define SRC_FUNESMEMORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets1/sockets.h>
#include <commons/config.h>
#include <commons/collections/list.h>

const char* IP = "127.0.0.1";
const int PUERTO_ESCUCHA = 8101;

typedef struct CPU_struct {
	int id;
	int socket;
} CPU_struct;

typedef struct tabla_segmento {
	int id;
	int base;
	int limite;
}tabla_segmento;

typedef struct direccion_logica{
	int segmento;
	int offset;
}direccion_logica;

typedef struct direccion_fisica{
	struct tabla_segmento* base;
	struct direccion_logica* offset;
}direccion_fisica;


t_config* config;
char* ipEscucha;
int puertoEscucha;
int socketEscucha, socketCpu, socketDam, idGDT;
int* id;

t_list* listaCpu;
t_list* lista_idGDTs;

void iniciarVariables();
void finalizarVariables();
void esperarConexiones();
void recibirGDT();
void guardarGDT();

#endif /* SRC_FUNESMEMORY_H_ */
