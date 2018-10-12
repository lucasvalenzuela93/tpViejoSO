/*
 ============================================================================
 Name        : sockets.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "sockets.h"



//Cliente
int conectarClienteA(int puerto, char* ip) {
	int socketDelServidor;
	struct addrinfo direccionDestino;
	struct addrinfo *informacionServidor;
	char* puertoDestino;

	puertoDestino = malloc(sizeof(int) + 1);
	// Definiendo el destino
	memset(&direccionDestino, 0, sizeof(direccionDestino));
	direccionDestino.ai_family = AF_INET;    // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	direccionDestino.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP
	sprintf(puertoDestino, "%d", puerto);

	getaddrinfo(ip, puertoDestino, &direccionDestino, &informacionServidor);  // Carga en server_info los datos de la conexion

	 // Creo el socket
	 if ((socketDelServidor = socket(informacionServidor->ai_family, informacionServidor->ai_socktype, informacionServidor->ai_protocol)) == -1) {
		 perror("socket");
		 exit(1);
	 }

	 if (connect(socketDelServidor, informacionServidor->ai_addr, informacionServidor->ai_addrlen) == -1) {
		 perror("connect");
		 exit(1);
	 }


	 freeaddrinfo(informacionServidor);  // No lo necesitamos mas
	 free(puertoDestino);
	 puts("Conectado a servidor");
	 return socketDelServidor;
}

//Servidor
//Crear socket, devuelvo un socket que se conecto
int socketServidor(int puerto, char* ip, int maxConexiones){
	struct sockaddr_in server;
	int miSocket;
	//asigno maximas conexiones permitidas
	MAX_CONEX = maxConexiones;
	//Funcion que crea el socket.
	miSocket = socket(AF_INET,SOCK_STREAM,0);
	//comprobacion de errores del socket
	if (miSocket == -1) {
		perror("Socket error");
		exit(1);
	}
	if (DEBUG_SOCKET) puts("Socket creado");

	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(puerto);
	if(inet_aton(ip, &server.sin_addr) == 0){
		puts("Direccion IP invalida");
		exit(1);
	}
	//Bind
	//Realizo bind, compruebo error
	if(bind(miSocket, (struct sockaddr *) &server, sizeof(server)) == -1){
		perror("Bind error");
		exit(1);
	}
	if (DEBUG_SOCKET) puts("Bind realizado");
	puts("Servidor en linea...");
	return miSocket;
}

int aceptarConexion(int miSocket) {
	int socketConectado;
	struct sockaddr_in conexion;

	//Listen
	//Escucho, y comprobacion errores
	if(listen(miSocket , MAX_CONEX) == -1){
		perror("Listen error");
		exit(1);
	}
	puts("Escuchando nuevas conexiones...");

	//Aceptar conexion
	//Ciclo de accept, bloquea el proceso hasta que cliente se concete
	int c = sizeof(struct sockaddr_in);
	socketConectado = accept(miSocket, (struct sockaddr *)&conexion,(socklen_t *) &c);

	if (socketConectado == -1) {
		perror("Accept error");
		exit(1);
	}
	if (DEBUG_SOCKET) puts("Conexion aceptada");

	return socketConectado;
}

int enviarInformacion(int socket, void *texto, int *bytesAMandar) {
	int totalEnviados = 0; // cuántos bytes se mandan ahora
	int bytesRestantes = *bytesAMandar; // cuántos se han quedado pendientes de antes, lo asigno a una variable local

	int bytesEnviados;
	while (totalEnviados < *bytesAMandar) {
		bytesEnviados = send(socket, texto + totalEnviados, bytesRestantes, 0);
		if (bytesEnviados == -1) {
			puts("Error...");
			break;
		}
		totalEnviados += bytesEnviados;
		bytesRestantes -= bytesEnviados;
	}

	*bytesAMandar = totalEnviados; // devuelve aquí la cantidad que se termino por mandar, se deberían haber mandado todos
	return bytesEnviados == -1 ? -1 : 0; // devuelve -1 si hay fallo, 0 si esta bien
}

int enviarDtb(int socket, DTB* estructura){
	if(!estructura){
		puts("Error estructura");
		return -1;
	}
	// serializo el DTB
	DTB_aux* aux = (DTB_aux*) malloc(sizeof(DTB_aux));
	aux->flagInicio = estructura->flagInicio;
	aux->idGdt = estructura->idGdt;
	aux->programCounter = estructura->programCounter;
	aux->socket = estructura->socket;
	int tamanioString = strlen(estructura->pathScript);
	aux->tamanioPath = tamanioString;
	if(enviarHeader(socket,"",ENVIAR_DTB) == -1){
		// error al enviar header.
		puts("Error al enviar header");
		return -1;
	}
	if(send(socket, aux,sizeof(DTB_aux), 0) <= 0){
		// error al enviar el DTB
		puts("Error al enviar DTB");
		return -1;
	}
	char* pathAEnviar = malloc(strlen(estructura->pathScript));
	strcpy(pathAEnviar, estructura->pathScript);
	enviarMensaje(socket, pathAEnviar);
	return 1;
}

DTB* recibirDtb(int socket){

	ContentHeader *header = recibirHeader(socket);
	if(header->id == ENVIAR_DTB){
		DTB_aux* aux = (DTB_aux*) malloc(sizeof(DTB_aux));

		int reciv = recv(socket, aux, sizeof(DTB_aux), 0);
		if(reciv <= 0){
			puts("error al recibir DTB aux");
			close(socket);
			free(aux);
			exit(1);
		}
		printf("Tamaño path: %d", aux->tamanioPath);
		char* path = malloc(aux->tamanioPath);
		recibirMensaje(socket, aux->tamanioPath, &path);
		DTB* dtb = (DTB*) malloc(sizeof(DTB));
		dtb->idGdt = aux->idGdt;
		dtb->flagInicio = aux->flagInicio;
		dtb->programCounter = aux->programCounter;
		dtb->socket = aux->socket;
		dtb->pathScript = malloc(strlen(path));
		strcpy(dtb->pathScript, path);
		free(path);
		return dtb;
	}
	return NULL;
}


int enviarHeader(int socketDestino, char* mensaje, int id) {
	int tamanioMensaje = strlen(mensaje);
	int tamanioHeader;
	ContentHeader * header = (ContentHeader*) malloc(sizeof(ContentHeader));

	header->largo = tamanioMensaje;
	header->id = id;
	tamanioHeader = sizeof(ContentHeader);

	if(enviarInformacion(socketDestino, header, &tamanioHeader) < 0){
		if (DEBUG_SOCKET) puts("Error en enviar header");
		free(header);
		return -1;
	}
	if (DEBUG_SOCKET) puts("Header enviado");

	free(header);
	return 1;
}

int enviarMensaje(int miSocket, char* mensaje){
	int tamanioMensaje = strlen(mensaje);
	return enviarInformacion(miSocket, mensaje, &tamanioMensaje);
	//deberia checkear aca o tirar error?
}

ContentHeader * recibirHeader(int socketEmisor){
	ContentHeader * header = (ContentHeader*) malloc(sizeof(ContentHeader));
	int recibido;

	recibido = recv(socketEmisor, header, sizeof(ContentHeader), 0);
	if (recibido < 0) {
		if (DEBUG_SOCKET) puts("Error en recibir header");
		exit(1);
	} else if (recibido == 0) {
		if (DEBUG_SOCKET) puts("Socket emisor desconectado header");
		close(socketEmisor);
		free(header);
		exit(1);
	}
	return header;
}

void recibirMensaje(int socketEmisor, int tamanioMensaje, char** bufferMensaje){
	int recibido;

	recibido = recv(socketEmisor, *bufferMensaje, tamanioMensaje, 0);
	if(recibido < 0){
		if (DEBUG_SOCKET) puts("Error en recibir mensaje");
		exit(1);
	} else if (recibido == 0){
		if (DEBUG_SOCKET) puts("Socket Emisor desconectado mensaje");
		close(socketEmisor);
		free(*bufferMensaje);
		exit(1);
	}
	(*bufferMensaje) [tamanioMensaje] = '\0';
	if(DEBUG_SOCKET) printf("El mensaje recibido *recibirMensaje* es: %s\n", *bufferMensaje);
}

int servidorConectarComponente(int socketEscucha, char* servidor, char* componente){
	int socketConectado;
	char *bufferMensaje;

	bufferMensaje = malloc(2 + sizeof(char) * strlen(componente));

	char* ok_servidor = malloc(2 + strlen(servidor));
	strcpy(ok_servidor, servidor);
	strcat(ok_servidor, "OK");

	char* ok_componente = malloc(2 + strlen(componente));
	strcpy(ok_componente, componente);
	strcat(ok_componente, "OK");

	socketConectado = aceptarConexion(socketEscucha);
	recibirMensaje(socketConectado, strlen(componente) * sizeof(char) + 2, &bufferMensaje);
	if (strcmp(bufferMensaje, ok_componente) != 0 || enviarMensaje(socketConectado, ok_servidor) < 0) {
		printf("Error conectando %s con %s\n", servidor, componente);
		close(socketConectado);
		close(socketEscucha);
		exit(1);
	}


	free(bufferMensaje);
	free(ok_servidor);
	free(ok_componente);
	printf(" %s y %s conetados...\n", servidor, componente);
	return socketConectado;
}

int clienteConectarComponente(char* cliente, char* componente, int puerto, char* ip) {

	int socketServ;
	char *bufferMensaje;
	bufferMensaje = malloc(2 * sizeof(char) + sizeof(char) * strlen(componente));

	char* ok_cliente = malloc(2 + strlen(cliente));
	strcpy(ok_cliente, cliente);
	strcat(ok_cliente, "OK");

	char* ok_componente = malloc(2 + strlen(componente));
	strcpy(ok_componente, componente);
	strcat(ok_componente, "OK");

	socketServ = conectarClienteA((int)puerto, ip);

	if (enviarMensaje(socketServ, ok_cliente) < 0) {
		printf("Error conectando %s con %s\n", cliente, componente);
		close(socketServ);
		exit(1);
	} else {
		recibirMensaje(socketServ, strlen(componente) * sizeof(char) + 2, &bufferMensaje);
		if (strcmp(bufferMensaje, ok_componente) != 0) {
			printf("Error conectando %s con %s\n", cliente, componente);
			close(socketServ);
			exit(1);
		}
	}
	printf(" %s y %s conetados...\n", cliente, componente);

	free(bufferMensaje);
	free(ok_cliente);
	free(ok_componente);
	return socketServ;
}
