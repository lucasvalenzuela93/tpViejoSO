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

void actualizarBitmap() {
	FILE* bitmap_f = fopen(string_from_format("%s/Metadata/Bitmap.bin", puntoMontaje), "w");
	fputs(string_substring(bitmap->bitarray, 0, cantBloques / 8), bitmap_f);
	fclose(bitmap_f);
}

int crearBloque() {
	int i;
	for (i = 0; i < bitmap->size * 8; i++) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			bitarray_set_bit(bitmap, i);
			FILE* f = fopen(string_from_format("%s/Bloques/%d.bin", puntoMontaje, i), "w");
			if (f == NULL)
				return -1;

			fclose(f);
			actualizarBitmap();
			log_debug(logger, "create_block: int=%d", i);
			return i;
		}
	}
	return -1;
}

bool agregarBloqueSiHaceFalta(char* path, int offset, int size) {
	char* pathFile = string_from_format("%s/Archivos/%s", puntoMontaje, path);
	t_config* configFile = config_create(pathFile);

	char* bufferArrayBloques = config_get_string_value(configFile, BLOQUES);
	bufferArrayBloques = string_substring_until(bufferArrayBloques, string_length(bufferArrayBloques) - 1);

	char** arrayBloques = config_get_array_value(configFile, BLOQUES);

	int finalBloque = ceil((double) (offset + size) / (double) tamBloque) - 1;

	int bloquesAsignados = 0;
	while (arrayBloques[bloquesAsignados] != NULL)
		bloquesAsignados++;

	while(bloquesAsignados <= finalBloque) {
		int posNuevoBloque = crearBloque();

		if (posNuevoBloque < 0) {
			log_debug(logger, "add_blocks_if_needed: bool=%d", false);
			return false;
		} else
			string_append_with_format(&bufferArrayBloques, ",%d", posNuevoBloque);

		bloquesAsignados++;
	}
	string_append(&bufferArrayBloques, "]");

	int fileSize = config_get_int_value(configFile, TAMANIO);

	FILE* f = fopen(pathFile, "w");
	fputs(string_from_format("TAMANIO=%d\n", fileSize), f);
	fputs(string_from_format("BLOQUES=%s", bufferArrayBloques), f);
	fclose(f);

	free(pathFile);
	config_destroy(configFile);
	free(bufferArrayBloques);
	int i = 0;
	while (arrayBloques[i] != NULL)
		free(arrayBloques[++i]);
	free(arrayBloques);

	return true;
}

void escribirBloque(int posicion, int offset, int length, char* buffer) {
	FILE* bloque = fopen(string_from_format("%s/Bloques/%d.bin", puntoMontaje, posicion), "r+");
	fseek(bloque, offset, SEEK_SET);
	fwrite(buffer, 1, length, bloque);
	fclose(bloque);
}

void actualizarTamanioArchivo(char* path, int offset, int size) {
	char* pathFile = string_from_format("%s/Archivos/%s", puntoMontaje, path);
	t_config* configFile = config_create(pathFile);

	char* arrayBloques = config_get_string_value(configFile, BLOQUES);
	int fileSize = config_get_int_value(configFile, TAMANIO);
	int diff = fileSize - (offset + size);

	if (diff < 0) {
		fileSize += abs(diff);

		FILE* f = fopen(pathFile, "w");
		fputs(string_from_format("TAMANIO=%d\n", fileSize), f);
		fputs(string_from_format("BLOQUES=%s", arrayBloques), f);
		fclose(f);
	}
}

bool guardarDatos(char* path, int offset, int size, char* buffer) {
	int tamInicial = size;
	if (!agregarBloqueSiHaceFalta(path, offset, size)) {
		log_debug(logger, "save_data: bool=%d", false);
		return false;
	}

	char* pathFile = string_from_format("%s/Archivos/%s", puntoMontaje, path);
	t_config* configFile = config_create(pathFile);

	char** arrayBloques = config_get_array_value(configFile, BLOQUES);

	int bloqueInicio = offset / tamBloque;
	int end_block = (offset + size) / tamBloque;

	int offset2 = offset - bloqueInicio * tamBloque;
	int len = (size > tamBloque) ? tamBloque : size;
	len -= offset2;
	escribirBloque(atoi(arrayBloques[bloqueInicio]), offset2, len, string_substring_until(buffer, len));
	free(arrayBloques[bloqueInicio]); //TODO si aca hago siempre free nunca me va a caer en el while de abajo ?
	bloqueInicio++;
	size -= len;

	int escrito = len;
	while (bloqueInicio <= end_block && arrayBloques[bloqueInicio] != NULL) {
		int b_len = (size > tamBloque) ? tamBloque : size;

		escribirBloque(atoi(arrayBloques[bloqueInicio]), 0, b_len, string_substring(buffer, escrito, b_len));
		bloqueInicio++;
		size -= b_len;
		escrito += b_len;
		free(arrayBloques[bloqueInicio]);
	}

	free(arrayBloques);
	config_destroy(configFile);
	free(pathFile);

//	actualizarTamanioArchivo(path, offset, tamInicial);

	log_debug(logger, "save_data: bool=%d", true);
	return true;
}

char* obtenerDatosDelBloque(int pos) {
	FILE* archivoBloque = fopen(string_from_format("%s/Bloques/%d.bin", puntoMontaje, pos), "r");

	int size;
	char* buffer;
	fseek(archivoBloque, 0L, SEEK_END);
	size = ftell(archivoBloque);
	fseek(archivoBloque, 0L, SEEK_SET);

	buffer = malloc(size);
	fread(buffer, size, 1, archivoBloque);
	buffer = string_substring_until(buffer, size);

	fclose(archivoBloque);

	return buffer;
}

char* obtenerDatos(char* path, int offset, int size) {
	char* pathFile = string_from_format("%s/Archivos/%s", puntoMontaje, path);
	t_config* configFile = config_create(pathFile);

	int bloqueInicio = offset / tamBloque;
	int bloqueFin = (offset + size) / tamBloque;

	if (config_get_int_value(configFile, TAMANIO) < offset + size) {
		free(pathFile);
		config_destroy(configFile);

		return "";
	}

	char** blocks_arr = config_get_array_value(configFile, BLOQUES);

	char* buffer = string_new();
	while (bloqueInicio <= bloqueFin && blocks_arr[bloqueInicio] != NULL) {
		int block_pos = atoi(blocks_arr[bloqueInicio]);
		char* block_buffer = obtenerDatosDelBloque(block_pos);
		int block_size = string_length(block_buffer);

		int len = (size - block_size < 0) ? size : block_size;
		char* block_result = string_substring_until(block_buffer, len);

		string_append(&buffer, block_result);

		free(block_buffer);
		free(block_result);
		free(blocks_arr[bloqueInicio]);

		bloqueInicio++;
		size -= len;
	}

	free(blocks_arr);
	config_destroy(configFile);
	return buffer;
}

bool validarArchivo(char* path) {
	struct stat sb;
	if(stat(path, &sb) == -1){
		return false;
	}
	return true;
}

void liberarBloques(char** arrayBloques) {
	int i = 0;
	while (arrayBloques[i] != NULL) {
		int bit_pos = atoi(arrayBloques[i]);
		bitarray_clean_bit(bitmap, bit_pos);
		i++;
	}

	actualizarBitmap();
}

bool borrarArchivo(char* path) {
	char* pathFile = string_from_format("%s/Archivos/%s", puntoMontaje, path);
	if (validarArchivo(pathFile)) {
		t_config* configFile = config_create(pathFile);
		char** blocks_arr = config_get_array_value(configFile, BLOQUES);
		config_destroy(configFile);

		liberarBloques(blocks_arr);
		remove(pathFile);
		return true;
	}
	return false;
}

bool crearArchivo(char* path, int cantLineas) {
	char* arrayBloques = string_new();
	int posBloque;
	int tamTotal = cantLineas * tamanioLinea;
	int bloquesNecesarios = tamTotal / tamBloque;
	if(tamTotal && tamBloque > 0){
		bloquesNecesarios ++;
	}
	if(validarArchivo(string_from_format("%s/Archivos/%s", puntoMontaje, path))){
		printf("Archivo %s ya existente\n", path);
		return false;
	}
	// TODO: ver si le division esta siempre va a ser entre potencias de dos
	int bloquesPorLinea = tamanioLinea / tamBloque;
	for(int i = 0; i < bloquesNecesarios; i++){
		posBloque = crearBloque();
		if(posBloque < 0) return false;
		if(i % bloquesPorLinea - 1 == 0 && i != 0){
			escribirBloque(i, 0, strlen("hola"), "hola");
		}
		string_append(&arrayBloques, string_from_format("%d",posBloque));
		if(i != bloquesNecesarios - 1){
			string_append(&arrayBloques, ",");
		}
	}
	char **pathSplit = string_split(path, "/");
	char *pathF = string_from_format("%s/Archivos/", puntoMontaje);
	int i = 0;
	while(pathSplit[i] != NULL){
		char* split = pathSplit[i];
		if(pathSplit[i + 1] == NULL){
			break;
		}
		string_append(&pathF, string_from_format("%s/", split));
		i ++;
	}
	if(strlen(pathF) > 1){
		if(!validarArchivo(pathF)){

			if(mkdir(pathF, 0700) && errno != EEXIST){
				puts("Error al crear el directorio");
			}
		}
	}
	FILE *file = fopen(string_from_format("%s/Archivos/%s", puntoMontaje, path), "w");
	if(file == NULL){
		puts("Error al crear el archivo");
		return false;
	}
	txt_write_in_file(file, string_from_format("BLOQUES=[%s]\n", arrayBloques));
	txt_write_in_file(file, string_from_format("TAMANIO=%d\n", tamTotal));
	fclose(file);

	return true;
}

void iniciarBitmap() {
	FILE* bitmapF = fopen(string_from_format("%s/Metadata/Bitmap.bin", puntoMontaje), "r");
	if(bitmapF == NULL){
		log_debug(logger, "Archivo Bitmap.bin inexistente");
		bitmapF = fopen(string_from_format("%s/Metadata/Bitmap.bin", puntoMontaje), "w+");
	}
	int size;
	char* buffer;
	fseek(bitmapF, 0L, SEEK_END);
	size = ftell(bitmapF);
	fseek(bitmapF, 0L, SEEK_SET);

	buffer = malloc(size);
	fread(buffer, size, 1, bitmapF);
	buffer = string_substring_until(buffer, size);

	//string_append(&buffer, string_repeat('\0', block_quantity / 8 - size));
	bitmap = bitarray_create_with_mode(buffer, cantBloques / 8, LSB_FIRST);

	//free(buffer);
	fclose(bitmapF);
}

void iniciarMetadata() {
	t_config* config_meta = config_create(string_from_format("%s/Metadata/Metadata.bin", puntoMontaje));
	if(config_meta == NULL){
		log_debug(logger, "Error al abrir Metadata");
		exit(1);
	}
	tamBloque = config_get_int_value(config_meta, TAMANIO_BLOQUES);
	cantBloques = config_get_int_value(config_meta, CANTIDAD_BLOQUES);
	magicNumber = config_get_string_value(config_meta, MAGIC_NUMBER);

	config_destroy(config_meta);
}

void iniciarVariables(){
	config = config_create(PATH_CONFIG);
	logger = log_create("log.txt","FILE_SYSTEM",true, LOG_LEVEL_INFO);
	if(config == NULL){
		puts("Error al levantar configuraciones...");
		exit(1);
	}

	ipEscucha = config_get_string_value(config, "IP_ESCUCHA");
	puertoEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE");
	retardo = config_get_int_value(config, "RETARDO");

	puts("Variables inicadas...");
}

void iniciarFileSystem() {
	iniciarVariables();
	iniciarMetadata();
	iniciarBitmap();
}

void finalizarVariables(){
	close(socketEscucha);
	close(socketDam);
}

void escucharMensajesDam(){
	puts("Escuchando mensajes del DAM");
	ContentHeader *header;
	while(true){
		header = recibirHeader(socketDam);
		switch(header->id){
			case DAM_CREAR:{
				int numLineas, id;
				// RECIBO ID DTB
				free(header);
				header = recibirHeader(socketDam);
				id = header->id;
				// RECIBO EL NUMEOR DE LINEAS
				free(header);
				header = recibirHeader(socketDam);
				numLineas = header->id;
				char* path = malloc(header->largo -1);
				// RECIBO EL PATH
				recibirMensaje(socketDam, header->largo, &path);
				free(header);
				if(!crearArchivo(path, numLineas)){
					puts("Error al crear el archivo");
					break;
				}
				enviarHeader(socketDam, "",MDJ_CREACION_ARCHIVO_OK);
				enviarHeader(socketDam,"", id);
				printf("Archivo: %s - CREADO\n\n", path);
				free(path);
				free(header);
				break;
			}
			case DAM_BORRAR:{
				puts("Dam borrar");
				char* path = malloc(header->largo);
				int idDtb;
				// RECIBO EL ID DEL DTB
				header = recibirHeader(socketDam);
				idDtb = header->id;
				// RECIBO EL PATH
				recibirMensaje(socketDam, header->largo, &path);
				if(borrarArchivo(path)){
					// EXISTE EL ARCHIVO
					puts("Archivo borrado");
					enviarHeader(socketDam, "", MDJ_BORRAR_OK);
					enviarHeader(socketDam, "", idDtb);
					break;
				}
				free(header);
				puts("No existe el archivo");
				break;
			}
		}
	}
	free(header);
}

int main(void) {
	puts("Iniciando File System...");
	// TEST
	tamanioLinea = 128;

	iniciarFileSystem();

	// ------TEST------
	crearArchivo("equipos/River.txt", 6);
//
//	sleep(2);
//
//	if(borrarArchivo("equipos/River.txt")){
//		puts("Archivo Borrado");
//	}else{
//		puts("Error al borrar archivo");
//	}

	socketEscucha = socketServidor(puertoEscucha, ipEscucha, 50);

	socketDam = servidorConectarComponente(socketEscucha, "FILE_SYSTEM", "DAM");

	// ESPERO QUE EL DAM ME DIGA EL TAMAÑO DE CADA LINEA
//	ContentHeader *header = recibirHeader(socketDam);
//	tamanioLinea = header->id;


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
