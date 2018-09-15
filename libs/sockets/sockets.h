/*
 * socket.h
 *
 *  Created on: 21 abr. 2018
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>

#ifndef SOCKET_H_
#define SOCKET_H_

typedef struct {
	t_log* logger;
	int socketDeEscucha;
	int backlog;
} __attribute__((packed)) t_parametros;

// Funciones de sockets que definen a los procesos como Servidor o Cliente

/* PARA QUE EL SERVIDOR OBTENGA SU PROPIA IP REVISAR:
 * http://www.forosdelweb.com/f96/como-obtener-mi-direccion-ip-con-c-c-381891/
 */

int conectarComoServidor(t_log* logger, const char* ip, const char* puerto);
int escucharCliente(t_log* logger, int listenningSocket);
void recibirMensajeConsola(t_log* logger, int socketCliente, int packagesize);

int conectarComoCliente(t_log* logger, const char* ip, const char* puerto);
void enviarMensajeConsola(t_log* logger, int socketServidor, int packagesize);
void enviarPaqueteNumerico(int socketDestino, int paqueteNumerico);

int finalizarSocket(int socket);

#endif /* SOCKET_H_ */
