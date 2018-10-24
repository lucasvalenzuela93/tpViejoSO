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
#include <commons/string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/chardefs.h>
#include <string.h>
#include <stdbool.h>
#include <sockets1/sockets.h>




typedef struct {
	int socketSafa;
	int socketDam;
	int socketFm9;
} parserSockets;

enum RESPUESTAS_PARSER {
	ABRIR_OK = 100,
	ABRIR_BLOQUEAR = 101,
	CONCENTRAR_OK = 102,
	ASIGNAR_ABORTAR_DTB = 103,
	ASIGNAR_OK = 104,
	WAIT_OK = 105,
	WAIT_ESPERAR = 106,
	SIGNAL_OK = 107,
	SIGNAL_ERROR = 108,
	FLUSH_ABORTAR = 109,
	FLUSH_OK = 110,
	CLOSE_ABORTAR = 110,
	CLOSE_OK = 111,
	CREAR_ABORTAR = 112,
	CREAR_OK = 113,
	BORRAR_ABORTAR = 114,
	BORRAR_OK = 115
};


int parsearLinea(char*, parserSockets*, DTB**);

#endif /* GENERALES_PARSER_H_ */
