/*
 * Server.h
 *
 *  Created on: 23/04/2014
 *      Author: maximilianofelice
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/collections/list.h>


#define PUERTO "6667"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define MAX_PACKAGE_SIZE 1024	//El servidor no admitira paquetes de mas de 1024 bytes
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300



/*
 * 	Definicion de estructuras
 *
 * 	Es importante destacar que utilizamos el tipo uint_32, incluida en el header <stdint.h> para mantener un estandar en la cantidad
 * 	de bytes del paquete.
 */
typedef struct _t_Package {
	char username[MAXUSERNAME];
	uint32_t username_long;
	char message[MAX_MESSAGE_SIZE];
	uint32_t message_long;
} t_Package;

pthread_t thread;
t_list * listaSockets;
/*
 * 	Definicion de funciones
 */
t_list * list_create();


#endif

int recieve_and_deserialize(t_Package *package, int socketCliente){

	int status;
	int buffer_size;
	char *buffer = malloc(buffer_size = sizeof(uint32_t));

	uint32_t username_long;
	status = recv(socketCliente, buffer, sizeof(package->username_long), 0);
	memcpy(&(username_long), buffer, buffer_size);
	if (!status) return 0;

	status = recv(socketCliente, package->username, username_long, 0);
	if (!status) return 0;

	uint32_t message_long;
	status = recv(socketCliente, buffer, sizeof(package->message_long), 0);
	memcpy(&(message_long), buffer, buffer_size);
	if (!status) return 0;

	status = recv(socketCliente, package->message, message_long, 0);
	if (!status) return 0;


	free(buffer);

	return status;
}
int crearConexion(char * puerto){
	    struct addrinfo hints;
		struct addrinfo *serverInfo;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;

		getaddrinfo(NULL, puerto, &hints, &serverInfo);


		int listenningSocket;
		listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);


		bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
		freeaddrinfo(serverInfo);
		return listenningSocket;

}
void iniciarChat(int socketCliente)
{
	t_Package package;
	int status = 1;
	recieve_and_deserialize(&package, socketCliente);
	printf("%s se ha unido al chat...\n", package.username);

	while (status){
		status = recieve_and_deserialize(&package, socketCliente);
		if (status){
			printf("%s says: %s", package.username, package.message);
            for(int i=0; i<list_size(listaSockets);i++)
            {
            	send(*(int *)list_get(listaSockets,i), package.message, strlen(package.message)+1, 0);
            }
		}

	}
	printf("%s se ha desconectado.\n", package.username);
	close(socketCliente);
}
void atenderCliente(int *socketCliente){
   iniciarChat(*socketCliente);
}

int main(){
    int socketDeEscucha;
    char * puerto = (char *)malloc(6);
    listaSockets=list_create();

    printf("Ingrese puerto desde el cual escuchar: ");
    scanf("%s",puerto);
    socketDeEscucha=crearConexion(puerto);

	listen(socketDeEscucha, BACKLOG);

	while(1)
	{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);


	//int socketCliente = accept(socketDeEscucha, (struct sockaddr *) &addr, &addrlen);
	int * socketCliente = (int *)malloc(sizeof(int));
	*socketCliente=accept(socketDeEscucha, (struct sockaddr *) &addr, &addrlen);
	list_add(listaSockets, socketCliente);
	pthread_create(&thread,NULL,(void*)atenderCliente,socketCliente);
	pthread_detach(thread);
	}

	close(socketDeEscucha);
	return 0;
}


