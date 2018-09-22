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

const char* IP_SAFA = "127.0.0.1";
const int PUERTO_SAFA = 8000;
const int PUERTO_ESCUCHA = 8001;
const int PUERTO_FILE_SYSTEM = 8100;
const int PUERTO_FUNES_MEMORY = 8101;

int socketSafa, socketEscucha, socketCpu, socketFileSystem, socketFunesMemory;


#endif /* SRC_DAM_H_ */
