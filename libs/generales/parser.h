/*
 * parser.h
 *
 *  Created on: 15 oct. 2018
 *      Author: utnso
 */

#ifndef GENERALES_PARSER_H_
#define GENERALES_PARSER_H_

#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/chardefs.h>
#include <string.h>
#include <stdbool.h>
#include <generales/generales.h>
#include <sockets1/sockets.h>

const char* ABRIR = "abrir";
const char* CONCENTRAR = "concentrar";
const char* ASIGNAR = "asignar";
const char* WAIT = "wait";
const char* SIGNAL = "signal";
const char* FLUSH = "flish";
const char* CLOSE = "close";
const char* CREAR = "crear";
const char* BORRAR = "borrar";

typedef struct {
	int socketSafa;
	int socketDam;
	int socketFm9;
} parserSockets;

enum RESPUESTAS_PARSER {
	ABRIR_OK = 1,
	ABRIR_BLOQUEAR = 2,
	CONCENTRAR_OK = 3,
	ASIGNAR_ABORTAR_DTB = 4,
	ASIGNAR_OK = 5,
	WAIT_OK = 6,
	WAIT_ESPERAR = 16,
	SIGNAL_OK = 7,
	SIGNAL_ERROR = 17,
	FLUSH_ABORTAR = 8,
	FLUSH_OK = 9,
	CLOSE_ABORTAR = 10,
	CLOSE_OK = 11,
	CREAR_ABORTAR = 12,
	CREAR_OK = 13,
	BORRAR_ABORTAR = 14,
	BORRAR_OK = 15
};

enum ERRORES_PARSER {
	PATH_INEXISTENTE = 10001,
	ESPACIO_INSUFICIENTE_FM9_ABRIR = 10002,
	ARCHIVO_CERRADO_ABRIR = 20001,
	FALLO_SEGMENTACION_ASIG = 20002,
	ESPACIO_INSUFICIENTE_FM9_ASIG = 20003,
	ARCHIVO_CERRADO_FLU = 30001,
	FALLO_SEGMENTACION_FLU = 30002,
	ESPACIO_INSUFICIENTE_MDJ = 30003,
	ARCHIVO_INEXISTENTE_FLU = 30004,
	ARCHIVO_CERRADO_CLO = 40001,
	FALLO_SEGMENTACION_CLO = 40002,
	ARCHIVO_YA_EXISTENTE = 50001,
	ESPACIO_INSUFICIENTE_CRE = 50002,
	ARCHIVO_INEXISTENTE_BORRAR = 60001
};

int parsearLinea(char*, parserSockets*, DTB*);

#endif /* GENERALES_PARSER_H_ */
