/*
 * Team.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>



int crearConexion(char * ip, char * puerto){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	getaddrinfo(ip, puerto, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
		/*
		 * 	Perfecto, ya tengo el medio para conectarme (el archivo), y ya se lo pedi al sistema.
		 * 	Ahora me conecto!
		 *
		 */
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas
	return serverSocket;

}
int main(){
	int socket1;
	char ip[12];
	char puerto [6];
	char mensaje [1024];
	printf("Ingrese IP del servidor al que desea conectarse: ");
	scanf("%s",&ip);
	printf("Ingrese puerto del servidor al que desea conectarse: ");
	scanf("%s", &puerto);
	socket1 = crearConexion(ip,puerto);
	printf("Contectado al servidor, ingrese su mensaje (exit para finalizar): ");
	while(1){
		    fgets(mensaje, 1024, stdin);
			if (!strcmp(mensaje,"exit\n")) break;
			send(socket1, mensaje, strlen(mensaje) + 1, 0);
		}


}
