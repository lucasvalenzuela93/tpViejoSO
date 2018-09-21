/*
 * safa.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef SRC_SAFA_H_
#define SRC_SAFA_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets/sockets.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>

const char* IP = "127.0.0.1";
const int PUERTO = 8000;

t_log *logger;

int socketEscucha,socket_cpu, socket_dam;

void inicializarVariables();
void* esperarMensajesDAM(void*);


#endif /* SRC_SAFA_H_ */
