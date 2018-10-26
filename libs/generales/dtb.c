/*
 * dtb.c
 *
 *  Created on: 22 oct. 2018
 *      Author: utnso
 */

#include "dtb.h"

resDtb* dtbToAux(DTB* dtb){
	char* res = string_new();
	char *recursos = string_new();
	char *archivos = string_new();

	void stringDeArchivos(void* a){
		char* arch = (char*) a;
		string_append(&archivos, string_from_format("%s%c",arch, '@'));
	}

	if(list_size(dtb->archivos) > 0){
		list_iterate(dtb->archivos, &stringDeArchivos);
		string_append(&(dtb->archivos), "\0");
	}

	dtbAux* aux = (dtbAux*) malloc(sizeof(dtbAux));
	aux->flagInicio = dtb->flagInicio;
	aux->idGdt = dtb->idGdt;
	aux->programCounter = dtb->programCounter;
	aux->socket = dtb->socket;
	aux->tamanioPath = strlen(dtb->pathScript);
	aux->tamanioArchivos = strlen(archivos);
	aux->rafaga = dtb->rafaga;

	resDtb* resultado = (resDtb*) malloc(sizeof(resDtb));

	resultado->archivos = string_duplicate(archivos);
	resultado->path = string_duplicate(dtb->pathScript);
	resultado->aux = aux;

	return resultado;
}


DTB* auxToDtb(resDtb* res){
	DTB* dtb = (DTB*) malloc(sizeof(DTB));

	dtb->idGdt = res->aux->idGdt;
	dtb->programCounter = res->aux->programCounter;
	dtb->flagInicio = res->aux->flagInicio;
	dtb->socket = res->aux->socket;
	dtb->rafaga = res->aux->rafaga;

	dtb->archivos = list_create();
	dtb->pathScript = string_duplicate(string_from_format("%s%c", res->path, '\0'));

	int i = 0;
	char** archivos = string_split(res->archivos, "@");
		if(archivos[0] != NULL){
			while(archivos[i] != NULL){
				list_add(dtb->archivos, archivos[i]);
				i ++;
			}
		}

	return dtb;
}
