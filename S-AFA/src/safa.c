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

int buscarDTBporId(void* dtbVoid,void* dtbId){
	DTB* dtb = (DTB*) dtbVoid;
	int id = atoi((char*) dtbId);
	return dtb->idGdt == id;
}

int filtrarCpu(void* cpuVoid,void* id){
	CPU_struct* cpu = (CPU_struct*) cpuVoid;
	int gdtId = (int) id;
	return cpu->gdtAsignado == gdtId;
}

int buscarCpuAsignado(void* cpuVoid, void* idGdt){
	CPU_struct* cpu = (CPU_struct*) cpuVoid;
	int id = (int) idGdt;
	return cpu->gdtAsignado = id;
}

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
	pthread_t hiloMensajes;
	if(pthread_create(&hiloMensajes,NULL,&manejarMensajes , NULL)){
		log_error(logger, "Error al crear hilo de mensajes");
		return 2;
	}
	pthread_t hiloColas;
	if(pthread_create(&hiloColas, NULL, &manejarColas,NULL)){
		log_error(logger, "Error al crear el hilo de colas");
		return 2;
	}
	puts("S-AFA esta operativo");
	iniciarConsola();
	if(pthread_join(hiloConexiones, NULL) != 0){
		puts("Error al joinear hilo de conexiones...");
		return 2;
	}
	if(pthread_join(hiloMensajes, NULL) != 0){
			log_error(logger, "Error al joinear el hilo de mensajes");
			return 2;
	}
	if(pthread_join(hiloColas, NULL) != 0){
		log_error(logger, "Error al joinear el hilo de colas");
		return 2;
	}
	finalizarVariables();


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
		multiprogramacion = config_get_int_value(config, "MULTIPROGRAMACION");
		algoritmo = config_get_string_value(config, "ALGORITMO");
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
	close(socketEscucha);
	config_destroy(config);
	list_destroy_and_destroy_elements(colaNew, free);
	list_destroy_and_destroy_elements(colaReady, free);
	list_destroy_and_destroy_elements(colaEjecucion, free);
	list_destroy_and_destroy_elements(colaBloqueados, free);
	list_destroy_and_destroy_elements(colaExit, free);
	list_destroy_and_destroy_elements(listaCpu, free);

	puts("Finalizo S-AFA correctamente...");
	exit(1);

}

void* manejarMensajes(){
	ContentHeader* header = (ContentHeader*) malloc(sizeof(ContentHeader));
	char* id;
	DTB* dtb;
	while(done == 0){
		header = recibirHeader(socket_dam);
		// en el mensaje me mandan el Dgt para q lo busque en la lista.

		switch(header->id){
			case SAFA_BLOQUEAR_CPU:{
				// debo recibir El id del Gdt y el Program counter del CPU y guardarlo en el dtb.
				header = recibirHeader(socket_dam);
				// primero recibo el id del Gdt
				char* pc,id;
				if(header->id == SAFA_ID_GDT_DEL_CPU){
					recibirMensaje(socket_dam, header->largo, &id);
					dtb->idGdt = atoi(id);
					dtb = list_remove_by_condition_with_param(colaNew, (void*) id, buscarDTBporId);

					header = recibirHeader(socket_dam);
					if(header->id == SAFA_PC_DEL_CPU){
						// recibo el program counter y lo guardo
						recibirMensaje(socket_dam, header->largo, &pc);
						dtb->programCounter = atoi(pc);
						list_add(colaBloqueados, (void*) dtb);
						// le saco el DTB asignado al cpu
						CPU_struct* cpu = (CPU_struct*) list_remove_by_condition_with_param(listaCpu, (void*) id,buscarCpuAsignado);
						cpu->gdtAsignado = -1;
						list_add(listaCpu, (void*) cpu);
					}
				}
				break;
			}
			case SAFA_DESBLOQUEAR_CPU:{
				recibirMensaje(socket_dam, header->largo, &id);
				// debo pasar el cpu de la lista de bloqueados a la lista de ready
				if(dtb = (DTB*)list_remove_by_condition_with_param(colaBloqueados, (void*) id, buscarDTBporId)){
					list_add(colaReady, (void*) dtb);
				}
				break;
			}
			case SAFA_MOVER_CPU_EXIT:{
				recibirMensaje(socket_dam, header->largo, &id);
				// debo mover el cpu que fallo a la cola de exit
				// TODO ver de que cola tengo que sacar el GDT cuando lo muevo a exit
				//	si de ready o de Ejecucion
				if(dtb = (DTB*)list_remove_by_condition_with_param(colaNew, (void*) id, buscarDTBporId)){
					list_add(colaExit, (void*) dtb);
					if(dtb->socket){
						// si el dtb tiene un cpu asignado le aviso q no ejecute mas.
						enviarHeader(dtb->socket,"",CPU_FRENAR_EJECUCION);
					}
				}
				break;
			}
			default: {
				break;
			}
		}
	}
	puts("Finalizo thread de mensajes");
}

void* manejarColas(){
	int totalEnMemoria;

	while(done == 0){
		totalEnMemoria = 0;
		totalEnMemoria += list_size(colaReady);
		totalEnMemoria += list_size(colaEjecucion);
		totalEnMemoria += list_size(colaBloqueados);
		if(totalEnMemoria < multiprogramacion && list_size(colaNew) > 0){
			// TENGO LUGAR EN LA COLA DE READY PARA PONER NUEVOS GDT
			// (EL GRADO DE MULTIPROGRAMACION ME LO PERMITE)
			DTB* aux = (DTB*) list_remove(colaNew, 0);
			list_add(colaReady, (void*) aux);
			continue;
		}
		if(strcmp(algoritmo,"FIFO") == 0){
			// compruebo si hay algun CPU sin un GDT asignado

			if(list_size(colaReady) > 0 && list_size(listaCpu) > 0){
				int sinAsignar = -1;
				CPU_struct *cpu;
				cpu = (CPU_struct*) list_remove_by_condition_with_param(listaCpu,(void*) sinAsignar ,filtrarCpu);
				if(cpu){
					// puedo asignar un CPU para que ejecute
					// saco el primer elemento de la cola ready para pasarlo a la cola de ejecucion
					DTB* dtb = (DTB*) list_remove(colaReady, 0);
					// le asigno el id del gdt al cpu y lo envio
					cpu->gdtAsignado = dtb->idGdt;
					enviarDtb(cpu->socket, dtb);
					dtb->socket = cpu->socket;
					list_add(colaEjecucion, (void*) dtb);
					list_add(listaCpu, (void*) cpu);
				}
			}
		}
	}
	puts("Finalizo thread de colas");
}

void conectarComponentes(){
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
			numeroClientes ++;

			if(numeroClientes > maxConex){
				// debo enviar mensaje para que finalice el CPU_struct
				enviarHeader(socketsCpu[numeroClientes -1], "", CPU_MAXIMAS_CONEXIONES_ALCANZADAS);
				close(socketsCpu[numeroClientes -1]);
				puts("Maxiamas conexiones alcanzadas...");
				numeroClientes --;
			}else {
				// debo enviar el id al CPU

				CPU_struct *CPU = (CPU_struct*) malloc(sizeof(CPU_struct));
				CPU->id = nextCpuId;
				CPU->socket = socketsCpu[numeroClientes -1];
				CPU->gdtAsignado = -1;
				enviarHeader(CPU->socket ,"", nextCpuId);

				list_add(listaCpu, (void*)CPU);
				nextCpuId ++;
				printf("Se conecto CPU con id: %d \n", CPU->id);

			}
		}

	}
	if(done != 0){
	while(numeroClientes > 0){
		CPU_struct *CPU = (CPU_struct*)list_get(listaCpu, numeroClientes -1);
		printf("El CPU %d fue finalizado por comando exit",CPU->id);
		close(socketsCpu[numeroClientes -1]);
		numeroClientes--;
		free(CPU);
	}
	close(socketEscucha);
	FD_ZERO(&descriptoresLectura);
	}
	puts("Finalizo thread de componentes");

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



int cmdFinalizar(char* id){
	DTB* dtb;
	if(dtb = (DTB*)list_find_with_param(colaEjecucion,(void*) id, buscarDTBporId)){
		// DEBO ESPERAR A QUE TERMINE DE EJECUTAR PARA PASARLO A LA COLA DE EXIT

	}else if(dtb = (DTB*) list_remove_by_condition_with_param(colaNew, (void*) id, buscarDTBporId)){
		list_add(colaExit, (void*) dtb);
	}
	else if(dtb = (DTB*) list_remove_by_condition_with_param(colaReady, (void*) id, buscarDTBporId)){
		list_add(colaExit, (void*) dtb);
	}
	else if(dtb = (DTB*) list_remove_by_condition_with_param(colaBloqueados, (void*) id, buscarDTBporId)){
		list_add(colaExit, (void*) dtb);
	}

}

int cmdStatusDTB(char* id){
	// DEBO MOSTRAR LOS DATOS DEL DTB
	DTB *dtb;
	if(dtb = (DTB*)list_find_with_param(colaNew,(void*) id, buscarDTBporId)){
		printf(
			"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i\n",
			dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio);
	}else if(dtb = (DTB*)list_find_with_param(colaReady,(void*) id, buscarDTBporId)){
			printf(
				"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i\n",
				dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio
			);
	}else if(dtb = (DTB*)list_find_with_param(colaEjecucion,(void*) id, buscarDTBporId)){
			printf(
				"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i\n",
				dtb->idGdt, dtb->pathScript, dtb->programCounter, dtb->flagInicio
			);
	}else if(dtb = (DTB*)list_find_with_param(colaBloqueados,(void*) id, buscarDTBporId)){
			printf(
				"ID: %i \n Path: %s \n Program Counter: %i \n Flag inicializacion: %i\n",
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

int cmdEjecutar(char* path){
	// FUCNIONS DE CONSOLA QUE EJECUTA UN SCRIPT
	DTB *dtb = (DTB*) malloc(sizeof(DTB));
	dtb->pathScript = malloc(strlen(path) + 1);
	strcpy(dtb->pathScript, path);
	dtb->pathScript[strlen(path)] = '\0';
	dtb->idGdt = idGdt;
	dtb->flagInicio = 0;
	dtb->programCounter = 0;

	list_add(colaNew,(void*) dtb);
	log_info(logger, "Se ha añadido un el G.DT %d a la cola de NEW", idGdt);
	idGdt ++;
	return 0;

}



