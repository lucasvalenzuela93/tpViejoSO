/*
 * dtb.h
 *
 *  Created on: 22 oct. 2018
 *      Author: utnso
 */

#ifndef GENERALES_DTB_H_
#define GENERALES_DTB_H_

#include <commons/collections/list.h>
#include <string.h>
#include <commons/string.h>

typedef struct DTB {
	int idGdt;
	char* pathScript;
	int programCounter;
	int flagInicio;
	int socket;
	int rafaga;
	t_list *archivos;
} __attribute__((packed)) DTB;

typedef struct dtbAux {
	int idGdt;
	int programCounter;
	int flagInicio;
	int socket;
	int tamanioPath;
	int tamanioArchivos;
	int rafaga;
} __attribute__((packed)) dtbAux;

typedef struct resDtb {
	dtbAux *aux;
	char* path;
	char* archivos;
} resDtb;


resDtb* dtbToAux(DTB*);
DTB* auxToDtb(resDtb*);

#endif /* GENERALES_DTB_H_ */
