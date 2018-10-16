/*
 * parser.c
 *
 *  Created on: 15 oct. 2018
 *      Author: utnso
 */

#include <parser.h>
int abrir(int socketDam, char* path, DTB* dtb){
	if(dtb->pathAbierto == 0){
		// LE AVISO AL DMA QUE BUSQUE EL ESCRIPTORIO
		enviarHeader(socketDam,"",CPU_PEDIR_ARCHIVO);
		// LE ENVIO EL ID DEL GDT Y EL PATH
		enviarHeader(socketDam,path ,SAFA_ID_GDT_DEL_CPU);
		enviarMensaje(socketDam, dtb->pathScript);
		enviarHeader(socketDam, "", dtb->idGdt);
		return ABRIR_BLOQUEAR;
	}
	return ABRIR_OK;
}

int asignar(int socketFm9, char** valores, DTB* dtb){
	if(dtb->pathAbierto == 0){
		return ASIGNAR_ABORTAR_DTB;
	}
	if(valores[1] && valores[2] && valores[3]){
		// DEBO ENVIAR AL FM9 EL PATH(SPLIT[1]), LA LINEA(SPLIT[2]), Y EL DATO(SPLIT[3])
	}
	return ASIGNAR_OK;
}

int wait(int socketSafa, char* recurso,DTB* dtb){
	// LE AVISO AL SAFA Q QUIERO PEDIR UN RECURSO
	enviarHeader(socketSafa,recurso,SAFA_PEDIR_RECURSO);
	enviarHeader(socketSafa,"",dtb->idGdt);
	enviarMensaje(socketSafa,recurso);
	// RECIBO LA RESPUESTA DEL SAFA A LA PETICION DEL RECURSO
	ContentHeader *header = recbirheader(socketSafa);
	if(header->id == RECURSO_OK){
		return WAIT_OK;
	}else if(header->id == RECURSO_RETENIDO){
		// LE DIGO AL SAFA QUE BLOQUEE AL DTB
		enviarHeader(socketSafa,"", SAFA_BLOQUEAR_CPU_RETENCION);
		enviarDtb(socketSafa, dtb);
		return WAIT_ESPERAR;
	}
}
int signal(int socketSafa, char* recurso, DTB* dtb){
	// LE AVISO A SAFA QUE QUIERO LIBERAR UN RECURSO
	enviarHeader(socketSafa,recurso,SAFA_LIBERAR_RECURSO);
	enviarHeader(socketSafa,"",dtb->idGdt);
	enviarMensaje(socketSafa,recurso);
	ContentHeader *header = recibirHeader(socketSafa);
	return header->id == RTA_OK ? SIGNAL_OK : SIGNAL_ERROR;
}

int concentrar(){
	// TODO funcion
	return CONCENTRAR_OK;
}
int flush(int socketDam,int socketSafa, char* path, DTB* dtb){
	if(dtb->pathAbierto == 0){
		return FLUSH_ABORTAR;
	}
	enviarHeader(socketDam,path,DAM_FLUSH);
	enviarMensaje(socketDam, path);

	// LE AVISO AL SAFA Q ME BLOQUEE
	enviarHeader(socketSafa,"", SAFA_BLOQUEAR_CPU_RETENCION);
	enviarDtb(socketSafa, dtb);
	return FLUSH_OK;
}

int close(int socketFm9, char* path, DTB* dtb){
	if(dtb->pathAbierto == 0){
		return CLOSE_ABORTAR;
	}
	// TODO: enviar al FM9 los datos para q saque de memoria el archivo asociado
	return CLOSE_OK;
}

int crear(int socketDam,int socketSafa, char* path, char* cantLineas, DTB* dtb){
	// ENVIO EL PATH Y EL NUMERO DE LINEAS AL DMA
	enviarHeader(socketDam, path, DAM_CREAR);
	enviarMensaje(socketDam,path);
	// ENVIO EL ID DEL DTB Y LUEGO LA CANTIDAD DE LINEAS
	enviarHeader(socketDam,"", dtb->idGdt);
	enviarHeader(socketDam,"",atoi(cantLineas));
	// BLOQUEO AL DTB
	enviarHeader(socketSafa,"", SAFA_BLOQUEAR_CPU_RETENCION);
	enviarDtb(socketSafa, dtb);
	return CREAR_OK;
}

int borrar(int socketDam,int socketSafa, char* path, DTB* dtb){
	// LE AVISO AL DAM QUE REENVIE UN MENSAJE DE BORRAR
	enviarHeader(socketDam, path, DAM_BORRAR);
	enviarMensaje(socketDam, path);
	// LUEGO DEL PATH LE PASO EL ID DEL DTB
	enviarHeader(socketDam, "", dtb->idGdt);
	// UNA VEZ ENVIADO TODOS LOS DATOS LE DIGO AL DAM QUE BLOQUEE EL DTB
	enviarHeader(socketSafa,"", SAFA_BLOQUEAR_CPU_RETENCION);
	enviarDtb(socketSafa, dtb);
	return BORRAR_OK;

}

int parsearLinea(char* linea, parserSockets *sockets, DTB* dtb){
	if(!string_starts_with(linea,"#")  || !string_starts_with(linea, '\n')){
		// CORROBORO QUE TERMINE CON UN '\n'
		if(linea[strlen(linea)] == '\n'){
			char** split = string_split(linea, " ");
			switch(split[0]){
			// PATH ABIERTO = 0 -> EL PATH ESTA CERRADO
			// PATH ABIERTO = 1 -> EL PATH ESTA ABIERTO
				case ABRIR:{
					return abrir(sockets->socketDam, split[1], dtb);
				}
				case CONCENTRAR:{
					return concentrar();
				}
				case ASIGNAR:{
					return asignar(sockets->socketFm9,split,dtb);
				}
				case WAIT:{
					return wait(sockets->socketSafa,split[1], dtb);
				}
				case SIGNAL:{
					return signal(sockets->socketSafa, split[1],dtb);
				}
				case FLUSH:{
					return flush(sockets->socketDam, sockets->socketSafa, split[1], dtb);
				}
				case CLOSE:{
					return close(sockets->socketFm9, split[1], dtb);
				}
				case CREAR:{
					return crear(sockets->socketDam, sockets->socketSafa, split[1], split[2], dtb);
				}
				case BORRAR:{
					return borrar(sockets->socketDam, sockets->socketSafa, split[1], dtb);
				}
				default: return 0;
			}
		}
	}
}
