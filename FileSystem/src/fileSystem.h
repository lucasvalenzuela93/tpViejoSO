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


const char* IP_ESCUCHA = "127.0.0.1";
const int PUERTO_ESCUCHA = 8100;

int socketEscucha, socketDam;

void finalizarVariables();


#endif /* SRC_FILESYSTEM_H_ */
