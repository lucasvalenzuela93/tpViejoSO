/*
 * fileSystem.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef SRC_FILESYSTEM_H_
#define SRC_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/chardefs.h>


const char* IP_ESCUCHA = "127.0.0.1";
const int PUERTO_ESCUCHA = 8100;

t_config* config;
char* ipEscucha;
int puertoEscucha;
int socketEscucha, socketDam;

void inciarVariables();
void finalizarVariables();

//Funciones de Consola
void iniciarConsola();
char *recortarLinea(char *);


#endif /* SRC_FILESYSTEM_H_ */
