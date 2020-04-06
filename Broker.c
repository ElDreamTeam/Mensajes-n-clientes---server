/*
 * Broker.c
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
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>





/*Crea un socket bindeado al puerto que se le pasa y lo retorna*/
int crearConexion(char * puerto){
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	getaddrinfo(NULL, puerto, &hints, &serverInfo);
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar
    return listenningSocket;
}
void atenderCliente(int *socketCliente){
	char username [1024];
	char package[1024];
	int status = 1;
	printf("Cliente conectado. Esperando mensajes:\n");
	while (status != 0){
				   	status = recv(*socketCliente, (void*) username, 1024, 0);
				   	if (status != 0) break;
			}
	status = 1;
	while (status != 0){
			   	status = recv(*socketCliente, (void*) package, 1024, 0);
			   	if (status != 0) printf("%s:%s",username, package);
		}
	close(*socketCliente);
}


/*void irAlChat(int socketCliente, char [] username){

}*/

int main()
{
	int socket1, value;
	char puerto [6];
	printf("Ingrese desde que puerto quiere escuchar: ");
    scanf("%s",&puerto);
    socket1=crearConexion(puerto);

    printf("Esperando conexion de cliente...\n");

    listen(socket1, 5);


    while(1)
    {

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    pthread_t thread;

    int socketCliente = accept(socket1, (struct sockaddr *) &addr, &addrlen);

    pthread_create(&thread,NULL,(void*)atenderCliente,&socketCliente);
    pthread_detach(thread);
    }



    close(socket1);

    return 0;


}

