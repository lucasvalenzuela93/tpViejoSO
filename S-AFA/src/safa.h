/*
 * safa.h
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */

#ifndef SRC_SAFA_H_
#define SRC_SAFA_H_

#include <stdio.h>
#include <stdlib.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>


const char* IP = "127.0.0.1";
const int PUERTO = 8000;

typedef struct COMANDO {
	char* cmd;
	int (*funcion)();
	char* info;
	int parametros;
} COMANDO;

int cmdHola(),cmdSalir(),cmdHelp();


t_log *logger;
int done = 0;

COMANDO comandos[] = {
		{"hola", cmdHola, "Mostrar hola", 0},
		{"salir", cmdSalir, "Termina el proceso", 0},
		{"help", cmdHelp, "Muestra los posibles comandos de consola", 0}
};

int socketEscucha,socket_cpu, socket_dam;

void inicializarVariables();
void* esperarMensajesDAM(void*);

void finalizarVariables();
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

#endif /* SRC_SAFA_H_ */
