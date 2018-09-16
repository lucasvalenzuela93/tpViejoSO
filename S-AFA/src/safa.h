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
#include <commons/log.h>
#include <commons/collections/list.h>

const char* IP = "127.0.0.1";
const char* PUERTO = "8100";

t_log *logger;

int socketEscucha;
t_list *socketsEscuchando;

void crearLogger();
void inicializarVariables();


#endif /* SRC_SAFA_H_ */
