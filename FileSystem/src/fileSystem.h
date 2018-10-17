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
#include <sys/types.h>
#include <sys/stat.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/chardefs.h>
#include <generales/generales.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

enum RTAS {
	ARCHIVO_EXISTENTE = 1,
	ARCHIVO_INEXISTENTE = 2
};

typedef struct COMANDO {
	char* cmd;
	int (*funcion)();
	char* info;
	int parametros;
} COMANDO;

COMANDO comandos[] = {

};

t_config* config;
t_log *logger;

char* magicNumber;
int tamBloque;
int cantBloques;

char* puntoMontaje;
int retardo;
char* ipEscucha;
int puertoEscucha;
int socketEscucha, socketDam;
int done = 0;
int tamanioLinea;
t_list *bitmap;

void leerBitmapBloques();
void iniciarVariables();
void finalizarVariables();
void escucharMensajesDam();
int validarArchivo(char*);
int crearArchivo(char*, int);

// ------------------FUNCIONES CONSOLA--------------------
void iniciarConsola(); 									// Ejecuta la consola.
void ejecutarComando(char *linea);				// Manda a ejecutar un comando.
void obtenerParametros(char **parametros, char *linea);	// Separa los parametros de la linea original.
char *leerComando(char *linea);		// Separa el comando de la linea original.
char *recortarLinea(char *string);// Quita los espacios al principio y al final de la linea.
int existeComando(char* comando);// Chequea que el comando exista en el array.
int ejecutarSinParametros(COMANDO *comando);// Llama a la funcion de un comando sin parametros.
int ejecutarConParametros(char *parametros, COMANDO *comando);// Llama a la funcion de un comando con parametros.
int verificarParametros(char *linea, int posicion);	// Chequea que la cantidad de parametros ingresada sea correcta.
COMANDO *punteroComando(int posicion);// Devuelve el puntero al comando del array.


#endif /* SRC_FILESYSTEM_H_ */
