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
#define QLEN 10 

#ifndef HOST_NAME_MAX 
#define HOST_NAME_MAX 256 
#endif	


//Funcion de ayuda para setear la bandera close on exec
void set_cloexec(int fd){
	if(fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) < 0){
		printf("error al establecer la bandera FD_CLOEXEC\n");	
	}
}


//Funcion para inicializar el servidor
int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen){

	int fd;
	int err = 0;
	
	if((fd = socket(addr->sa_family, type, 0)) < 0)
		return -1;
		
	if(bind(fd, addr, alen) < 0)
		goto errout;
		
	if(type == SOCK_STREAM || type == SOCK_SEQPACKET){
		
			if(listen(fd, qlen) < 0)
				goto errout;
	}
	return fd;
errout:
	err = errno;
	close(fd);
	errno = err;
	return (-1);
}


//Damos el servicio
void serve(int sockfd) { 
	int clfd; FILE *fp; 
	char buf[ BUFLEN]; 

	set_cloexec( sockfd); 

	for (;;) { 
		if (( clfd = accept( sockfd, NULL, NULL)) < 0) { 		//Aceptamos una conexion
			syslog( LOG_ERR, "ruptimed: accept error: %s", strerror( errno)); 	//si hay error la ponemos en la bitacora			
			exit( 1); 
		} 

		set_cloexec(clfd); 

		if (( fp = popen("/usr/bin/uptime", "r")) == NULL) { 		//llamamos al programa uptime con un pipe
			sprintf( buf, "error: %s\n", strerror( errno)); 
			send( clfd, buf, strlen( buf), 0); 
		} 
		else { 
			while (fgets( buf, BUFLEN, fp) != NULL) 		//lo que nos haya devuelto uptime, lo mandamos al cliente
				send( clfd, buf, strlen(buf), 0); 
			pclose( fp); 						//cerramos el pipe
		}
		close(clfd); 							//cerramos la conexion con el cliente.
	} 
}



//Main

int main( int argc, char *argv[]) { 
	int sockfd, n;
	char *host; 

	if(argc == 1){
		printf("Uso: ./servidor <numero de puerto>\n");
		exit(-1);
	}

	if(argc != 2){
		printf( "por favor especificar un numero de puerto\n");
	}

	int puerto = atoi(argv[1]);

	if (( n = sysconf(_SC_HOST_NAME_MAX)) < 0) 		
		n = HOST_NAME_MAX; /* best guess */ 
	if ((host = malloc(n)) == NULL) 
		printf(" malloc error"); 
	if (gethostname( host, n) < 0) 		//Obtenemos nombre del host
		printf(" gethostname error"); 
	
	printf("Nombre del host: %s\n", host);	//Mostramos nuestro nombre

	

	//Direccion del servidor
	struct sockaddr_in direccion_servidor;

	memset(&direccion_servidor, 0, sizeof(direccion_servidor));	//ponemos en 0 la estructura direccion_servidor

	//llenamos los campos
	direccion_servidor.sin_family = AF_INET;		//IPv4
	direccion_servidor.sin_port = htons(puerto);		//Convertimos el numero de puerto al endianness de la red
	direccion_servidor.sin_addr.s_addr = inet_addr("127.0.0.1") ;	//Nos vinculamos a la interface localhost o podemos usar INADDR_ANY para ligarnos A TODAS las interfaces

	

	//inicalizamos servidor (AF_INET + SOCK_STREAM = TCP)
	if( (sockfd = initserver(SOCK_STREAM, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor), 1000)) < 0){	//Hasta 1000 solicitudes en cola 
		printf("Error al inicializar el servidor\n");	
	}		

	while(1){
		serve(sockfd);
		//TODO servimos
	}
	

	
	exit( 1); 
}


