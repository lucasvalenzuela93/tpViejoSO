/*
 * dam.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef SRC_DAM_H_
#define SRC_DAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

t_config *config;
int puertoEscucha, puertoSafa, puertoFileSystem, puertoFunesMemory;
char* ipEscucha;
char* ipSafa;
char* ipFileSystem;
char* ipFunesMemory;
int socketSafa, socketEscucha, socketCpu, socketFileSystem, socketFunesMemory;

void iniciarVariables();
void finalizarVariables();

#endif /* SRC_DAM_H_ */
