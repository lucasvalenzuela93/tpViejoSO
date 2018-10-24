/*
 * cpu.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef SRC_CPU_H_
#define SRC_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <generales/generales.h>
#include <parser/parser.h>

const char* IP = "127.0.0.1";
const int PUERTO = 8000;
const int PUERTO_DAM = 8001;
const int PUERTO_FUNES_MEMORY = 8101;



t_log *logger;
t_config* config;
char* ipSafa;
char* ipDam;
char* ipFunesMemory;
int puertoSafa, puertoDam, puertoFunesMemory;
int socketSAFA, socketDam, socketFunesMemory;
int max_linea;
pthread_t *hiloMensajes;
InfoCpu *self;

int ejecutar = 0;

void inciarVariables();
void finalizarVariables();
void escucharMensajesSafa();
int recibirTamMaxLinea();


#endif /* SRC_CPU_H_ */
