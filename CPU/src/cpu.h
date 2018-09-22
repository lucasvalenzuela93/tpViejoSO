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
#include <sockets/sockets.h>
#include <sockets1/sockets.h>
#include <commons/log.h>

const char* IP = "127.0.0.1";
const int PUERTO = 8000;
const int PUERTO_DAM = 8001;
const int PUERTO_FUNES_MEMORY = 8101;

t_log *logger;
int socketSAFA, socketDam, socketFunesMemory;

void crearLogger();


#endif /* SRC_CPU_H_ */
