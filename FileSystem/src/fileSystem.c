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

char* convertirPath(char* path){
	char* pathAbsoluto = malloc(strlen(path) + strlen(puntoMontaje));
	if(string_ends_with(path, "/") || string_ends_with(puntoMontaje, "/")){
		sprintf(pathAbsoluto,".%s%s", puntoMontaje, path);
	}else {
		sprintf(pathAbsoluto,".%s/%s", puntoMontaje, path);
	}
	return pathAbsoluto;
}

int buscarBitmapLibre(void* bloqueVoid){
	int bloque = (int) bloqueVoid;
	return bloque != 1;
}

int main(void) {
	puts("Iniciando File System...");
	// TEST
	tamanioLinea = 128;

	iniciarVariables();

//	crearArchivo("Arqueros.bin",5);

	socketEscucha = socketServidor(puertoEscucha, ipEscucha, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FILE_SYSTEM", "DAM");

	// ESPERO QUE EL DAM ME DIGA EL TAMAÑO DE CADA LINEA
	ContentHeader *header = recibirHeader(socketDam);
	tamanioLinea = header->id;


	// CREO EL HILO DE ESCUCHA DE MENSAJES
	pthread_t hiloMensajes;
	if(pthread_create(&hiloMensajes, NULL, escucharMensajesDam , NULL)){
		log_error(logger, "Error al crear hilo de mensajes");
		return 2;
	}

	iniciarConsola();
	if(pthread_join(hiloMensajes,NULL) != 0){
		log_error(logger, "Error al joinear hilo de mensajes");
		finalizarVariables();
		exit(1);
	}
	finalizarVariables();
	puts("Finalizo File System...");
	return EXIT_SUCCESS;
}

void iniciarVariables(){
	config = config_create("./config/config.txt");
	logger = log_create("log.txt","FILE_SYSTEM",true, LOG_LEVEL_INFO);
	if(config == NULL){
		puts("Error al levantar configuraciones...");
		exit(1);
	}

	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE");
	retardo = config_get_int_value(config, "RETARDO");

	// LEVANTO METADATA DE FILE SYSTEM
	char* pathMetadata = malloc(strlen(puntoMontaje) + strlen("Metadata/metadata.bin") + 1);
	strcpy(pathMetadata, ".");
	strcat(pathMetadata, puntoMontaje);
	strcat(pathMetadata, "Metadata/metadata.bin");
	t_config *configMetadata = config_create(pathMetadata);
	if(configMetadata == NULL){
		log_error(logger, "Error al obtener metadata");
		exit(1);
	}
	magicNumber = config_get_string_value(configMetadata, "MAGIC_NUMBER");
	tamBloque = config_get_int_value(configMetadata,"TAMANIO_BLOQUES");
	cantBloques = config_get_int_value(configMetadata, "CANTIDAD_BLOQUES");
	magicNumber[strlen(magicNumber) + 1] = '\0';
	actualizarBitmap();

	puts("Variables inicadas...");
}

void actualizarBitmap(){
	bitmap = list_create();
		// RECORRO EL BITMAP PARA VER QUE BLOQUES ESTAN OCUPADOS
		int estado;
		char* path, *pathBitmap;
		int bitmapString[cantBloques];
		pathBitmap = malloc(strlen("Metadata/Bitmap.bin") + strlen(puntoMontaje) + 1);
		sprintf(pathBitmap, ".%sMetadata/Bitmap.bin", puntoMontaje);
		FILE* bitmapFile = fopen(pathBitmap, "w+b");
		if(bitmapFile == NULL){
			free(pathBitmap);
			log_error(logger, "Error al leer Bitmap");
			exit(1);
		}
		char *bit = malloc(sizeof(int));
		for(int i = 0; i < cantBloques; i++){
			int bloque = list_get(bitmap,i);
			path = malloc(contarDigitos(bloque) + strlen(".bin") + strlen("Bloques/"));
			sprintf(path, "Bloques/%d.bin",i);
			if(validarArchivo(convertirPath(path)) == ARCHIVO_EXISTENTE) estado = 1;
				else estado = 0;
			list_add_in_index(bitmap, i, (void*) estado);
			sprintf(bit,"%d", estado);
			fwrite(bit,1,strlen(bit), bitmapFile);
			free(path);
		}

		fclose(bitmapFile);

}

void finalizarVariables(){
	close(socketEscucha);
	close(socketDam);
}

void escucharMensajesDam(){
	ContentHeader *header = recibirHeader(socketDam);
	switch(header->id){
		case DAM_ABRIR:{

			break;
		}
	}
}

int validarArchivo(char* path){
	struct stat sb;
	if(stat(path, &sb) == -1){
		return ARCHIVO_INEXISTENTE;
	}

	return ARCHIVO_EXISTENTE;
}

int crearArchivo(char* path, int cantLineas){
	t_config *configBloque;
	char* arrayBloques, *pathBloque, *tamanioTotArchivo, *buffer;
	char bufferF[100];
	int tamTotal = cantLineas * tamanioLinea;
	// AVERIGUO LA CANTIDAD DE BLOQUES
	int restoBloques = tamTotal % tamBloque;
	int cantidadBloques = restoBloques > 0 ? tamTotal/tamBloque + 1 : tamTotal/tamBloque;
	// SI EL ARCHIVO NO EXISTE LO CREO
	char* pathFinal = malloc(strlen("Archivos/") +  strlen(path));
	sprintf(pathFinal, "Archivos/%s", path);
	configBloque = malloc(sizeof(t_config));
	configBloque->path = strdup(convertirPath(pathFinal));
	configBloque->properties = dictionary_create();
	if(validarArchivo(configBloque->path) == ARCHIVO_EXISTENTE){
		free(configBloque);
		return -1;
	}
	// LOS CORCHETES Y LAS COMAS PARA EL ARCHIVO DE CONFIG
	arrayBloques = malloc(cantidadBloques + cantidadBloques + 1);
	strcpy(arrayBloques,"[");
	if(configBloque != NULL){
		// RECORRO EL BITMAP DE BLOQUES PARA SABER CUALES ESTAN ASIGNADOS
		// Y DARLE LOS PRIMERO X BLOQUES Q REQUIERA EL ARCHIVO
		int bloquesAsignados = 0;
		for(int i = 0; i < list_size(bitmap); i++){
			int bloque = (int) list_get(bitmap,i);
			// CAMBIO EL VALOR DEL BLOQUE EN EL BITMAP
			if(bloque != 1){
				bloquesAsignados ++;
				list_replace(bitmap, i, (void*) 1);
				pathBloque = malloc(contarDigitos(i) + strlen(".bin") + strlen("/Bloques/"));
				sprintf(pathBloque,"Bloques/%d.bin", i);
				// CREO EL BLOQUE
				if(validarArchivo(convertirPath(pathBloque)) == ARCHIVO_INEXISTENTE){
					int fd2 = open(convertirPath(pathBloque), O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
					close(fd2);
				}
				// AGREGO EL NUMERO DE BLOQUE AL ARRAY DE BLOQUES
				sprintf(buffer, "%d", i);
				strcat(arrayBloques, buffer);
				if(i != cantidadBloques -1){
					strcat(arrayBloques,",");
				}

				if(bloquesAsignados == cantidadBloques){
					break;
				}
			}
		}
		strcat(arrayBloques, "]");
		tamanioTotArchivo = malloc(contarDigitos(tamTotal) + 1);
		sprintf(bufferF,"%d", tamTotal);
		tamanioTotArchivo = (char*) bufferF;
		// GUARDO LOS DATOS DEL ARCHIVO
		if(!config_has_property(configBloque, "BLOQUES")){
			config_set_value(configBloque, "BLOQUES", arrayBloques);
		}
		if(!config_has_property(configBloque, "TAMANIO")){
			char* bufferTam = malloc(contarDigitos(tamTotal));
			sprintf(bufferTam,"%d",tamTotal);
			config_set_value(configBloque, "TAMANIO",bufferTam);
		}

		config_save(configBloque);
		free(pathFinal);
		free(pathBloque);
		free(tamanioTotArchivo);
		free(arrayBloques);
		return 1;
	}

	return -1;
}


// -------------------------- FUNCIONES DE CONSOLA----------
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
		if(comandos[posicion].cmd == "status" && comandos[posicion].parametros == 0){
			verificarParametros(linea, posicion +1);
		}else {
			printf("%s: La cantidad de parametros ingresados es incorrecta.\n",
				comandos[posicion].cmd);
		}

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
