/*
 * parser.c
 *
 *  Created on: 15 oct. 2018
 *      Author: utnso
 */

#include "parser.h"

int abrir(int socketDam, char* path, DTB* dtb){
//	if(dtb->pathAbierto == 0){
		// LE AVISO AL DMA QUE BUSQUE EL ESCRIPTORIO
		enviarHeader(socketDam,"",CPU_PEDIR_ARCHIVO);
		// LE ENVIO EL ID DEL GDT Y EL PATH
		enviarHeader(socketDam,path ,SAFA_ID_GDT_DEL_CPU);
		enviarMensaje(socketDam, dtb->pathScript);
		enviarHeader(socketDam, "", dtb->idGdt);
		return ABRIR_BLOQUEAR;
//	}
	return ABRIR_OK;
}

int asignar(int socketFm9, char** valores, DTB* dtb){
//	if(dtb->pathAbierto == 0){
//		return ASIGNAR_ABORTAR_DTB;
//	}
	if(valores[1] && valores[2] && valores[3]){
		// DEBO ENVIAR AL FM9 EL PATH(SPLIT[1]), LA LINEA(SPLIT[2]), Y EL DATO(SPLIT[3])
	}
	return ASIGNAR_OK;
}

int wait(int socketSafa, char* recurso,DTB* dtb){
	printf("\tWAIT -- %s\n", recurso);
	// LE AVISO AL SAFA Q QUIERO PEDIR UN RECURSO
	enviarHeader(socketSafa,recurso,SAFA_PEDIR_RECURSO);
	enviarHeader(socketSafa,recurso,dtb->idGdt);
	enviarMensaje(socketSafa,recurso);
	return 3;
}
int signal(int socketSafa, char* recurso, DTB* dtb){
	printf("\tSIGNAL -- %s --Id: %d\n", recurso, dtb->idGdt);
	// LE AVISO A SAFA QUE QUIERO LIBERAR UN RECURSO
	enviarHeader(socketSafa,recurso,SAFA_LIBERAR_RECURSO);
	enviarHeader(socketSafa,recurso,dtb->idGdt);
	enviarMensaje(socketSafa,recurso);
	return 3;
}

int concentrar(){
	// TODO funcion
	return CONCENTRAR_OK;
}
int flush(int socketDam,int socketSafa, char* path, DTB* dtb){
//	if(dtb->pathAbierto == 0){
//		return FLUSH_ABORTAR;
//	}
	enviarHeader(socketDam,path,DAM_FLUSH);
	enviarMensaje(socketDam, path);
	// DEVUELVO 1 PARA QUE SE BLOQUEE EL DTB
	return 1;
}

int closeOp(int socketFm9, char* path, DTB* dtb){
//	if(dtb->pathAbierto == 0){
//		return CLOSE_ABORTAR;
//	}
	// TODO: enviar al FM9 los datos para q saque de memoria el archivo asociado
	return CLOSE_OK;
}

int crear(int socketDam,int socketSafa, char* path, char* cantLineas, DTB* dtb){
	printf("\tCREAR -- %s\n", path);
	// ENVIO EL PATH Y EL NUMERO DE LINEAS AL DMA
	enviarHeader(socketDam, path, DAM_CREAR);
	enviarMensaje(socketDam,path);
	// ENVIO EL ID DEL DTB Y LUEGO LA CANTIDAD DE LINEAS
	enviarHeader(socketDam,"", dtb->idGdt);
	int lineas = atoi(cantLineas);
	enviarHeader(socketDam,"",lineas);
	// DEVUELVO 1 PARA QUE SE BLOQUEE EL DTB
	return 1;
}

int borrar(int socketDam,int socketSafa, char* path, DTB* dtb){
	printf("\tBORRAR -- %s\n", path);
	// LE AVISO AL DAM QUE REENVIE UN MENSAJE DE BORRAR
	enviarHeader(socketDam, path, DAM_BORRAR);

	// LUEGO DEL PATH LE PASO EL ID DEL DTB
	enviarHeader(socketDam, path, dtb->idGdt);
	// ENVIO EL MENSAJE
	enviarMensaje(socketDam, path);
	// DEVUELVO 1 PARA QUE SE BLOQUEE EL DTB
	return 1;

}

int parsearLinea(char* linea, parserSockets *sockets, DTB* dtbo){
	if(!string_starts_with(linea,"#")){
		char** split = string_split(linea, " ");
		char* operacion = split[0];
		/*
		 * 	RETURNS:
		 * 		 1 - pedir bloqueo
		 * 		 2 - continuar
		 * 		-1 - error
		 * 		 3 - esperar respuesta
		 */
		if(strcmp(operacion, "abrir") == 0){
			return abrir(sockets->socketDam, split[1], dtbo);
		}
		if(strcmp(operacion, "concentrar") == 0){
			return concentrar();
		}
		if(strcmp(operacion, "asignar") == 0){
			return asignar(sockets->socketFm9,split,dtbo);
		}
		if(strcmp(operacion, "wait") == 0){
			return wait(sockets->socketSafa, split[1], dtbo);
		}
		if(strcmp(operacion, "signal") == 0){
			return signal(sockets->socketSafa, split[1],dtbo);
		}
		if(strcmp(operacion, "flush") == 0){
			return flush(sockets->socketDam, sockets->socketSafa, split[1], dtbo);
		}
		if(strcmp(operacion, "close") == 0){
			return closeOp(sockets->socketFm9, split[1], dtbo);
		}
		if(strcmp(operacion, "crear") == 0){
			if(split[2] == NULL){
				puts("Error al parsear e split");
			}
			char* linea = string_duplicate((char*) split[2]);
			return crear(sockets->socketDam, sockets->socketSafa, split[1], linea, dtbo);
		}
		if(strcmp(operacion, "borrar") == 0){
//			char* path = string_duplicate(split[1]);
			return borrar(sockets->socketDam, sockets->socketSafa, split[1], dtbo);
		}
		return -1;
	}
}
