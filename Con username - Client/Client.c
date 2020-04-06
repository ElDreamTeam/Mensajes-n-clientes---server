#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

#define IP "127.0.0.1"
#define PUERTO "6667"
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300

/*
 * 	Definicion de estructuras
 *
 * 	Es importante destacar que utilizamos el tipo uint_32, incluida en el header <stdint.h> para mantener un estandar en la cantidad
 * 	de bytes del paquete.
 *
 */
typedef struct _t_Package {
	char* username;
	uint32_t username_long;
	char* message;
	uint32_t message_long;
	uint32_t total_size;			// NOTA: Es calculable. Aca lo tenemos por fines didacticos!
} t_Package;

pthread_t thread;
/*
 * 	Definicion de funciones
 */

char* serializarOperandos(t_Package*);
void get_Username(char**);
void fill_package(t_Package*, char**);
void dispose_package(char**);

#endif /* CLIENTE_H_ */
int crearConexion(char * ip, char * puerto){
		struct addrinfo hints;
		struct addrinfo *serverInfo;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		getaddrinfo(ip, puerto, &hints, &serverInfo);



		int serverSocket;
		serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);


		connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
		freeaddrinfo(serverInfo);	// No lo necesitamos mas
		return serverSocket;


}
void recibirMensajes(int *socketServidor){
	while(1){
		char * buffer = malloc(1024);
		recv(*socketServidor, buffer, 1024, 0);
		printf("A %s",buffer);
	}
}
int main(){

	int socketServidor;
	char * username = malloc(MAXUSERNAME);
	char * ip = (char *)malloc(16);
	char * puerto = (char *)malloc(6);
	get_Username(&username);
	printf("Ingrese IP del servidor al que desea conectarse: ");
	scanf("%s",ip);
	printf("Ingrese puerto del servidor al que desea conectarse: ");
	scanf("%s", puerto);

	socketServidor = crearConexion(ip,puerto);





	printf("Conectado al servidor.\n");

	int enviar = 1;
	t_Package package;
	package.message = malloc(MAX_MESSAGE_SIZE);
	char *serializedPackage;


	printf("Bienvenido al sistema, puede comenzar a escribir. Escriba 'exit' para salir.\n");

	pthread_create(&thread,NULL,(void*)recibirMensajes,&socketServidor);
	pthread_detach(thread);
	while(enviar){

		fill_package(&package, &username);

		if(!strcmp(package.message, "exit\n")) enviar = 0;

		if(enviar) {
			serializedPackage = serializarOperandos(&package);
			send(socketServidor, serializedPackage, package.total_size, 0);
			dispose_package(&serializedPackage);
		}
	}

	printf("Desconectado\n");


	free(package.message);
	free(username);


	close(socketServidor);

	return 0;

}

char* serializarOperandos(t_Package *package){

	char *serializedPackage = malloc(package->total_size);

	int offset = 0;
	int size_to_send;

	size_to_send =  sizeof(package->username_long);
	memcpy(serializedPackage + offset, &(package->username_long), size_to_send);
	offset += size_to_send;

	size_to_send =  package->username_long;
	memcpy(serializedPackage + offset, package->username, size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(package->message_long);
	memcpy(serializedPackage + offset, &(package->message_long), size_to_send);
	offset += size_to_send;

	size_to_send =  package->message_long;
	memcpy(serializedPackage + offset, package->message, size_to_send);

	return serializedPackage;
}


/*
 * 	Funciones auxiliares
 */

void get_Username(char **username){

	printf("Ingrese su nombre de usuario: ");

	fgets(*username, MAXUSERNAME, stdin);

	/* Como fgets incluye el \n al final del username, nosotros se lo sacamos: */
	int username_long = strlen(*username);
	(*username)[username_long-1] = '\0';

}

void fill_package(t_Package *package, char** username){
	/* Me guardo los datos del usuario y el mensaje que manda */
	package->username = *username;
	package->username_long = strlen(*username) + 1; 		// Me guardo lugar para el \0

	fgets(package->message, MAX_MESSAGE_SIZE, stdin);
	(package->message)[strlen(package->message)] = '\0';

	package->message_long = strlen(package->message) + 1;	// Me guardo lugar para el \0

	package->total_size = sizeof(package->username_long) + package->username_long + sizeof(package->message_long) + package->message_long;
	// Si, este ultimo valor es calculable. Pero a fines didacticos la calculo aca y la guardo a futuro, ya que no se modificara en otro lado.
}

void dispose_package(char **package){
	free(*package);
}
