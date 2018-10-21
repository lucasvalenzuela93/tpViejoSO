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


int parsearLinea(char*, parserSockets*, DTB*);

#endif /* GENERALES_PARSER_H_ */
