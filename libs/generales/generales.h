/*
 * generales.h
 *
 *  Created on: 12 oct. 2018
 *      Author: utnso
 */

#ifndef GENERALES_GENERALES_H_
#define GENERALES_GENERALES_H_

typedef struct {
	int id;
	int rafaga;
} __attribute__((packed))InfoCpu;

int contarDigitos(int);

//// ECLIPSE
//#define PATH_CONFIG "./config/config.txt"
// CONSOLA
#define PATH_CONFIG "./config/config.txt"
#define PATH_CONFIG_FUNES_MEMORY "/home/utnso/workspace/tp-2018-2c-keAprobo/FunesMemory/config/config.txt"
#define PATH_CONFIG_DAM "/home/utnso/workspace/tp-2018-2c-keAprobo/DAM/config/config.txt"

#endif /* GENERALES_GENERALES_H_ */
