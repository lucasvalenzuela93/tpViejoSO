/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "fileSystem.h"

int main(void) {
	puts("Iniciando File System...");
	iniciarConsola();
	//iniciarVariables();
	char *linea, *aux;
	linea = readline("utnso@ubuntu-server:~$ ");

	socketEscucha = socketServidor(PUERTO_ESCUCHA, IP_ESCUCHA, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FILE_SYSTEM", "DAM");

	finalizarVariables();
	puts("Finalizo File System...");
	return EXIT_SUCCESS;
}




//------------------- FUNCIONES DE CONSOLA ------------------------

//void iniciarConsola() {
//	char *linea, *aux;
//	int done = 0;
//	while (done == 0) {
//		linea = readline("utnso@ubuntu-server:~$ ");
//		if (!linea)
//			break;
//		aux = recortarLinea(linea);
//		if (*aux)
//			//ejecutarComando(aux);
//		free(linea);
//	}
//}


char *recortarLinea(char *string) {
	register char *s, *t;
	for (s = string; whitespace(*s); s++);
	if (*s == 0)
		return s;
	t = s + strlen(s) - 1;
	while (t > s && whitespace(*t))
		t--;
	*++t = '\0';
	return s;
}









void iniciarVariables(){
	//config = config_create("./config/config.txt");
	config = config_create("/home/utnso/workspace/tp-2018-2c-keAprobo/FileSystem/config/config.txt");
	if(config == NULL){
		puts("Error al levantar configuraciones...");
		finalizarVariables();
		exit(1);
	}

	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");

	puts("Variables inicadas...");
}

void finalizarVariables(){
	shutdown(socketEscucha,2);
	shutdown(socketDam,2);
}
