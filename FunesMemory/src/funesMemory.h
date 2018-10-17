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
const int MAX_LINEA = 128; //Sólo para pruebas. Luego este parámetro se pasará desde archivo config

typedef struct CPU_struct {
	int id;
	int socket;
} CPU_struct;

t_config* config;
char* ipEscucha;
int puertoEscucha;
int socketEscucha, socketCpu, socketDam;
t_list *listaCpu;

void iniciarVariables();
void finalizarVariables();
void esperarConexiones();

#endif /* SRC_FUNESMEMORY_H_ */
