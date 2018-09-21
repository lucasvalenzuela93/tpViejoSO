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
		if (bytesEnviados == -1) { break; }
		totalEnviados += bytesEnviados;
		bytesRestantes -= bytesEnviados;
	}

	*bytesAMandar = totalEnviados; // devuelve aquí la cantidad que se termino por mandar, se deberían haber mandado todos
	return bytesEnviados == -1 ? -1 : 0; // devuelve -1 si hay fallo, 0 si esta bien
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
		if (DEBUG_SOCKET) puts("Socket emisor desconectado");
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
		if (DEBUG_SOCKET) puts("Socket Emisor desconectado");
		close(socketEmisor);
		free(*bufferMensaje);
		exit(1);
	}
	(*bufferMensaje) [tamanioMensaje] = '\0';
	if (DEBUG_SOCKET) printf("El mensaje recibido *recibirMensaje* es: %s\n", *bufferMensaje);
}

int servidorConectarComponente(int socketEscucha, char* servidor, char* componente){
	int socketConectado;
	char *bufferMensaje, *texto;

	bufferMensaje = malloc(3);
	texto = malloc(3 * sizeof(char));
	strcpy(texto, "OK");

	socketConectado = aceptarConexion(socketEscucha);
	recibirMensaje(socketConectado, 2 * sizeof(char), &bufferMensaje);

	if (strcmp(bufferMensaje, "OK") != 0 || enviarMensaje(socketConectado, texto) < 0) {
		printf("Error conectando %s con %s\n", servidor, componente);
		close(socketConectado);
		close(socketEscucha);
		exit(1);
	}


	free(bufferMensaje);
	free(texto);
	printf(" %s y %s estan conetados...\n", servidor, componente);
	return socketConectado;
}

int clienteConectarComponente(char* cliente, char* componente, int puerto, char* ip) {

	int socketServ;
	char *bufferMensaje, *texto;

	bufferMensaje = malloc(3 * sizeof(char));
	texto = malloc(3 * sizeof(char));
	strcpy(texto, "OK");

	socketServ = conectarClienteA((int)puerto, ip);

	if (enviarMensaje(socketServ, texto) < 0) {
		printf("Error conectando %s con %s\n", cliente, componente);
		close(socketServ);
		exit(1);
	} else {
		recibirMensaje(socketServ, 2 * sizeof(char), &bufferMensaje);
		if (strcmp(bufferMensaje, "OK") != 0) {
			printf("Error conectando %s con %s\n", cliente, componente);
			close(socketServ);
			exit(1);
		}
	}
	printf(" %s y %s estan conetados...\n", cliente, componente);

	free(bufferMensaje);
	free(texto);
	return socketServ;
}
