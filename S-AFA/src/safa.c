/*
 ============================================================================
 Name        : libreriaKeAprobo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "safa.h"

int main(void) {
	inicializarVariables();


	socketEscucha = socketServidor(puertoEscucha, IP, 50);


	socket_dam = servidorConectarComponente(socketEscucha, "S-AFA", "DAM");
	// CREO THREAD PARA QUE ESPERE MENSAJES DEL DAM
	pthread_t hiloDam;
	char* componente = malloc(4);
	strcpy(componente, "DAM");
	if(pthread_create(&hiloDam,NULL, esperarMensajesDAM, &socket_dam ) != 0){
		// ERROR AL CREAR EL HILO
		puts("Error al crear hilo que escucha al DAM...");
	}

	socket_cpu = servidorConectarComponente(socketEscucha, "S-AFA", "CPU");
	iniciarConsola();


	puts("Finalizo S-AFA...");
	return EXIT_SUCCESS;
}

void inicializarVariables(){
	puts("Iniciando S-AFA...");
	logger = log_create("log.txt","S-AFA",true, LOG_LEVEL_INFO);
	// LEVANTO CONFIGURACIONES
	config = config_create(PATH_CONFIG);
	if(config != NULL){
		IP = config_get_string_value(config, "IP");
		puertoEscucha = config_get_int_value(config,"PUERTO");
	}else {
		puts("Error al crear configuracion");
	}


	puts("Variables inicializadas...");
}

void finalizarVariables(){
	close(socket_cpu);
	close(socket_dam);
	close(socketEscucha);
	free(logger);
	free(IP);
	free(puertoEscucha);
	config_destroy(config);

}

void* esperarMensajesDAM(void* socket){
	int socket_dam = (int) socket;
	printf("%d", socket_dam);

	return NULL;
}

//-------------------FUNCIONES DE CONSOLA------------------------
int existeComando(char* comando) {
	register int i;
	for (i = 0; comandos[i].cmd; i++) {
		if (strcmp(comando, comandos[i].cmd) == 0) {
			return i;
		}
	}
	return -1;
}

char *leerComando(char *linea) {
	char *comando;
	int i, j;
	int largocmd = 0;
	for (i = 0; i < strlen(linea); i++) {
		if (linea[i] == ' ')
			break;
		largocmd++;
	}
	comando = malloc(largocmd + 1);
	for (j = 0; j < largocmd; j++) {
		comando[j] = linea[j];
	}
	comando[j++] = '\0';
	return comando;
}

void obtenerParametros(char **parametros, char *linea) {
	int i, j;
	for (i = 0; i < strlen(linea); i++) {
		if (linea[i] == ' ')
			break;
	}
	(*parametros) = malloc(strlen(linea) - i);
	i++;
	for (j = 0; i < strlen(linea); j++) {
		if (linea[i] == '\0')
			break;
		(*parametros)[j] = linea[i];
		i++;
	}
	(*parametros)[j++] = '\0';
}

COMANDO *punteroComando(int posicion) {
	return (&comandos[posicion]);
}

int ejecutarSinParametros(COMANDO *comando) {
	return ((*(comando->funcion))());
}

int ejecutarConParametros(char *parametros, COMANDO *comando) {
	return ((*(comando->funcion))(parametros));
}

int verificarParametros(char *linea, int posicion) {
	int i;
	int espacios = 0;
	char *parametros;
	COMANDO *comando;
	for (i = 0; i < strlen(linea); i++) {
		if (linea[i] == ' ')
			espacios++;
	}
	if (comandos[posicion].parametros == espacios) {
		if (espacios == 0) {
			comando = punteroComando(posicion);
			ejecutarSinParametros(comando);
		} else {
			obtenerParametros(&parametros, linea);
			comando = punteroComando(posicion);
			ejecutarConParametros(parametros, comando);
			free(parametros);
		}
	} else {
		printf("%s: La cantidad de parametros ingresados es incorrecta.\n",
				comandos[posicion].cmd);
	}
	return 0;
}

void ejecutarComando(char *linea) {
	char *comando = leerComando(linea);
	int posicion = existeComando(comando);
	if (posicion == -1) {
		printf("%s: El comando ingresado no existe.\n", comando);
	} else {
		verificarParametros(linea, posicion);
	}
	free(comando);
}

char *recortarLinea(char *string) {
	register char *s, *t;
	for (s = string; whitespace(*s); s++)
		;
	if (*s == 0)
		return s;
	t = s + strlen(s) - 1;
	while (t > s && whitespace(*t))
		t--;
	*++t = '\0';
	return s;
}

void iniciarConsola() {
	char *linea, *aux;
	done = 0;
	while (done == 0) {
		linea = readline("user@SAFA: ");
		if (!linea)
			break;
		aux = recortarLinea(linea);
		if (*aux)
			ejecutarComando(aux);
		free(linea);
	}
}

// ----------------------------- COMANDOS CONSOLA ------

int cmdHola(){
	puts("hola");
	return 0;
}
int cmdSalir(){
	done = 1;
	finalizarVariables();
	return 0;
}
int cmdHelp() {
	register int i;
	puts("Comando:\t\t\tDescripcion:");
	for (i = 0; comandos[i].cmd; i++) {
		if (strlen(comandos[i].cmd) < 7)
			printf("%s\t\t\t\t%s\n", comandos[i].cmd, comandos[i].info);
		else
			printf("%s\t\t\t%s\n", comandos[i].cmd, comandos[i].info);
	}
	return 0;
}


