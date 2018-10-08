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
	pthread_t hiloConexiones;
	if(pthread_create(&hiloConexiones, NULL, conectarComponentes, NULL)){
		puts("Error al crear el hilo de conexiones...");
	}
	while(estado == CORRUPTO){
		if(socket_dam && (list_size(listaCpu) != 0)){
			estado = OPERATIVO;
			break;
		}
		continue;
	}
	puts("S-AFA esta operativo");
	iniciarConsola();
	if(pthread_join(hiloConexiones, NULL)){
		puts("Error al joinear hilo de conexiones...");
		return 2;
	}
	finalizarVariables();
	puts("Finalizo S-AFA correctamente...");

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
		maxConex = config_get_int_value(config, "MAX_CONEX");
	}else {
		log_error(logger, "Error al cargar configuraciones");
	}
	estado = CORRUPTO;
	listaCpu = list_create();
	colaNew = list_create();
	colaReady = list_create();
	colaEjecucion = list_create();
	colaBloqueados = list_create();
	colaExit = list_create();

	puts("Variables inicializadas...");
}

void finalizarVariables(){
	close(socket_dam);
	config_destroy(config);
	list_destroy_and_destroy_elements(colaNew, free);
	list_destroy_and_destroy_elements(colaReady, free);
	list_destroy_and_destroy_elements(colaEjecucion, free);
	list_destroy_and_destroy_elements(colaBloqueados, free);
	list_destroy_and_destroy_elements(colaExit, free);
	list_destroy_and_destroy_elements(listaCpu, free);

}

void* conectarComponentes(){
	// declaro variables
	fd_set descriptoresLectura;
	struct timeval timeout;
	int numeroClientes = 0;
	int nextCpuId = 1;
	int socketsCpu[maxConex];

	while(done == 0){
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		// inicio descriptoresLectura
		FD_ZERO(&descriptoresLectura);
		// se añade para select() el socket servidor
		FD_SET(socketEscucha, &descriptoresLectura);
		//añado los CPU_struct ya conectados
		for(int i = 0; i< numeroClientes; i++){
			FD_SET(socketsCpu[i], &descriptoresLectura);
		}
		// escucho a nuevas conexiones
		select(100, &descriptoresLectura, NULL,NULL,&timeout);

		// se comprueba si algun cliente nuevo desea conectarse
		if(FD_ISSET(socketEscucha, &descriptoresLectura)){
			socketsCpu[numeroClientes] = servidorConectarComponente(socketEscucha, "S-AFA", "CPU");
			numeroClientes++;

			if(numeroClientes > maxConex){
				// debo enviar mensaje para que finalice el CPU_struct
				enviarHeader(socketsCpu[numeroClientes -1], "", CPU_MAXIMAS_CONEXIONES_ALCANZADAS);
				close(socketsCpu[numeroClientes -1]);
				puts("Maxiamas conexiones alcanzadas...");
				numeroClientes --;

			}else {
				// debo enviar el id al CPU
				enviarHeader(socketsCpu[numeroClientes -1],ID, nextCpuId);

				CPU_struct *CPU = malloc(sizeof(CPU_struct));
				CPU->id = nextCpuId;
				CPU->socket = socketsCpu[numeroClientes -1];
				list_add(listaCpu, CPU);
				nextCpuId ++;
				printf("Se conecto CPU con id: %d \n", CPU->id);
			}
		}

	}
	while(numeroClientes > 0){
		CPU_struct *CPU = (CPU_struct*)list_get(listaCpu, numeroClientes -1);
		printf("El CPU %d fue finalizado por comando exit",CPU->id);
		close(socketsCpu[numeroClientes -1]);
		numeroClientes--;
		free(CPU);
	}
	close(socketEscucha);
	free(socketsCpu);
	FD_ZERO(&descriptoresLectura);
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


int cmdStatus(){
	// DEBO MOSTRAR TODAS LAS COLAS
	puts("Cola NEW:");
	if(list_size(colaNew) == 0){
		puts("Vacia...");
	}else {
		puts("Id\tPath\t\t\tPC\tFlag Inicio\t");
		list_iterate(colaNew, imprimirCola);
		puts("-------------------------------------");
	}

	puts("Cola READY");
	if(list_size(colaReady) == 0){
		puts("Vacia...");
	}else {
		puts("Id\tPath\t\t\tPC\tFlag Inicio\t");
		list_iterate(colaReady, imprimirCola);
		puts("-------------------------------------");
	}
	puts("Cola Ejecucion");
	if(list_size(colaEjecucion) == 0){
		puts("Vacia...");
	}else {
		puts("Id\tPath\t\t\tPC\tFlag Inicio\t");
		list_iterate(colaEjecucion, imprimirCola);
		puts("-------------------------------------");
	}
	puts("Cola Bloqueados");
	if(list_size(colaBloqueados) == 0){
		puts("Vacia...");
	}else {
		puts("Id\tPath\t\t\tPC\tFlag Inicio\t");
		list_iterate(colaBloqueados, imprimirCola);
		puts("-------------------------------------");
	}
	puts("Cola Exit");
	if(list_size(colaExit) == 0){
		puts("Vacia...");
	}else {
		puts("Id\tPath\t\t\tPC\tFlag Inicio\t");
		list_iterate(colaExit, imprimirCola);
		puts("-------------------------------------");
	}


	return 0;

}

int buscarDTBporId(void* dtbVoid,void* dtbId){
	DTB* dtb = (DTB*) dtbVoid;
	int id = atoi((char*) dtbId);
	return dtb->idGdt == id;
}

int cmdStatusDTB(char* id){
	// DEBO MOSTRAR LOS DATOS DEL DTB
	printf("Debo mostrar los datos del DTB con id: %s\n", id);
	DTB *dtb;
	if(dtb = (DTB*)list_find_with_param(colaNew,(void*) id, buscarDTBporId)){
		printf(
			"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i",
			dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio);
	}else if(dtb = (DTB*)list_find_with_param(colaReady,(void*) id, buscarDTBporId)){
			printf(
				"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i",
				dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio
			);
	}else if(dtb = (DTB*)list_find_with_param(colaEjecucion,(void*) id, buscarDTBporId)){
			printf(
				"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i",
				dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio
			);
	}else if(dtb = (DTB*)list_find_with_param(colaBloqueados,(void*) id, buscarDTBporId)){
			printf(
				"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i",
				dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio
			);
	} else {
		puts("El id ingresado no existe...");
	}

}

void imprimirCola(void* elem){
	DTB* elemento = (DTB*) elem;
	printf("%i\t%s\t\t\t%i\t%i\n", elemento->idGdt, elemento->pathScript, elemento->programCounter, elemento->flagInicio);
}




