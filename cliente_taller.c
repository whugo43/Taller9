#include <sys/types.h>          /* some systems still require this */
#include <sys/stat.h>
#include <stdio.h>              /* for convenience */
#include <stdlib.h>             /* for convenience */
#include <stddef.h>             /* for offsetof */
#include <string.h>             /* for convenience */
#include <unistd.h>             /* for convenience */
#include <signal.h>             /* for SIG_ERR */ 
#include <netdb.h> 
#include <errno.h> 
#include <syslog.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>


#define BUFLEN 128 
#define MAXSLEEP 64

int connect_retry( int domain, int type, int protocol, 	const struct sockaddr *addr, socklen_t alen){
	
	int numsec, fd; /* * Try to connect with exponential backoff. */ 

	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) { 

		if (( fd = socket( domain, type, protocol)) < 0) 
			return(-1); 

		if (connect( fd, addr, alen) == 0) { /* * Conexión aceptada. */ 
			return(fd); 
		} 
		close(fd); 				//Si falla conexion cerramos y creamos nuevo socket

		/* * Delay before trying again. */
		if (numsec <= MAXSLEEP/2)

			sleep( numsec); 
	} 
	return(-1); 
}


void print_uptime( int sockfd) { 
	int n; 
	char buf[ BUFLEN]; 

	while (( n = recv( sockfd, buf, BUFLEN, 0)) > 0) 				
		write( STDOUT_FILENO, buf, n); 			//Imprimimos lo que recibimos
	if (n < 0) 	
		printf(" recv error"); 
}



int main( int argc, char *argv[]) { 

	int sockfd;

	if(argc == 1){
		printf("Uso: ./cliente <ip> <puerto>\n");
		exit(-1);
	}

	if(argc != 3){
		printf( "por favor especificar un numero de puerto\n");
	}

	int puerto = atoi(argv[2]);


	//Direccion del servidor
	struct sockaddr_in direccion_cliente;

	memset(&direccion_cliente, 0, sizeof(direccion_cliente));	//ponemos en 0 la estructura direccion_servidor

	//llenamos los campos
	direccion_cliente.sin_family = AF_INET;		//IPv4
	direccion_cliente.sin_port = htons(puerto);		//Convertimos el numero de puerto al endianness de la red
	direccion_cliente.sin_addr.s_addr = inet_addr(argv[1]) ;	//Nos tratamos de conectar a esta direccion

	//AF_INET + SOCK_STREAM = TCP

	if (( sockfd = connect_retry( direccion_cliente.sin_family, SOCK_STREAM, 0, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente))) < 0) { 
		printf("falló conexión\n"); 
		exit(-1);
	} 

	//En este punto ya tenemos una conexión válida
	print_uptime(sockfd);

	return 0; 
}


