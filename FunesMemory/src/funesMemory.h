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

const char* IP = "127.0.0.1";
const int PUERTO_ESCUCHA = 8101;

int socketEscucha, socketCpu, socketDam;


void finalizarVariables();

#endif /* SRC_FUNESMEMORY_H_ */
