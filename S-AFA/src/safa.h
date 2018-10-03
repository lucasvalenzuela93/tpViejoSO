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
#include <ctype.h>
#include <sockets1/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/chardefs.h>
#include <string.h>



const char* PATH_CONFIG = "./config/config.txt";
const char* CORRUPTO = "Corrupto";
const char* OPERATIVO = "Operativo";


typedef struct COMANDO {
	char* cmd;
	int (*funcion)();
	char* info;
	int parametros;
} COMANDO;


// falta completar con datos del CPU
typedef struct CPU_struct {
	int id;
	int socket;
} CPU_struct;

typedef struct DTB {
	int idGdt;
	char* pathScript;
	int programCounter;
	int flagInicio;
	t_list *archivosAbiertos;
} DTB;

int cmdHola(),cmdSalir(),cmdHelp(), cmdEjecutar(),cmdStatus(),cmdStatusDTB();

t_config* config;
t_log *logger;
int done = 0;
int puertoEscucha;
char* IP;
char* estado;
int maxConex;
int idGdt = 1;
t_list *listaCpu;
t_list *colaNew;
t_list *colaReady;
t_list *colaEjecucion;
t_list *colaBloqueados;
t_list *colaExit;

COMANDO comandos[] = {
		{"hola", cmdHola, "Mostrar hola", 0},
		{"salir", cmdSalir, "Termina el proceso", 0},
		{"ejecutar", cmdEjecutar, "Ejecuta el script pasado por parametro", 1},
		{"status", cmdStatus, "Muestra los valores de las colas por pantalla", 0},
		{"status", cmdStatusDTB, "Muestra los valores de las colas por pantalla", 1},
		{"help", cmdHelp, "Muestra los posibles comandos de consola", 0}
};

int socketEscucha,socket_cpu, socket_dam;

void inicializarVariables();
void* conectarComponentes();

void finalizarVariables();
void imprimirCola();
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
