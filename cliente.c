/*
 Archivo: cliente.c

 Nombres:
	Alejandra Cordero / Carnet: 12-10645
	Ricardo Mena  / Carnet: 12-10872

 Ultima modificacion: 09/05/2015

*/


#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <time.h>

int main(int argc, char *argv[])
{
	int serverPort;
	int sockfd;
	int addr_len,numbytes,numbytes2; /* conteo de bytes a escribir */ 
	int numeroIntentos;
	char *identificador;
	char *operacion;
	char mensaje[80];
	char recibido[30];

	struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */ 
	struct hostent *direccionDestino; 
	struct timeval timeout; 


	/* code */
	if (argc != 9){

		printf("Los argumentos no fueron pasados de forma correcta.\n");
		exit(0);
	}
	else{
		int i ;
		div_t j;

		for (i = 1; i < argc -1 ; ++i)
		{
			j = div(i,2);

			if (j.rem != 0 ){

				if ( strcmp(argv[i],"-p") == 0) {
					serverPort = atoi(argv[i+1]);

				}
				else if ( strcmp(argv[i],"-d") == 0){

					/* convertimos el hostname a su direccion IP */ 
					if ((direccionDestino=gethostbyname(argv[i+1])) == NULL) { 
						perror("gethostbyname"); 
						exit(1); 
					} 

				}
				else if ( strcmp(argv[i],"-c") == 0){

					if (( strcmp(argv[i+1],"s") == 0)||( strcmp(argv[i+1],"e") == 0)) {
						operacion = argv[i+1];
					}
					else{
						printf("Los argumentos no fueron pasados de forma \
							   correcta.\n");
						exit(0);	
					}

				}

				else if ( strcmp(argv[i],"-i") == 0){
					identificador = argv[i+1];

				}

				else{

					printf("Los argumentos no fueron pasados de forma \
						    correcta.\n");
					exit(0);
				}
			}

		}


	}

	// Se crea el socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
		perror("socket"); 
		exit(2); 
	} 

	// Se configura el timeout del socket a 2 seg.     
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
	            sizeof(timeout)) < 0)
	    perror("setsockopt failed\n");

	/* a donde mandar */ 
	their_addr.sin_family = AF_INET; /* usa host byte order */ 
	their_addr.sin_port = htons(serverPort); /* usa network byte order */ 
	their_addr.sin_addr = *((struct in_addr *)direccionDestino->h_addr); 
	bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */ 


    // Se arma el mensaje que se le enviara al servidor.
    memset(mensaje, 0, sizeof mensaje); // Se limpia el arrecho de caracteres.
    strcat(mensaje,identificador);  	// Indetificador del vehiculo
    strcat(mensaje,",");
    strcat(mensaje,operacion);			// Operacion que realizara el vehiculo

	printf("mensaje :%s \n", mensaje);
	
	// Se envia el mensaje con toda la informacion del cliente al servidor
	if ((numbytes=sendto(sockfd,mensaje,strlen(mensaje),0,
		(struct sockaddr *)&their_addr,sizeof(struct sockaddr))) == -1) { 
		perror("sendto"); 
		exit(2); 
	} 

	// Se espera el mensaje del servidor.
	numbytes2 = recvfrom(sockfd,recibido,30,0, (struct sockaddr *)&their_addr,
		(socklen_t *)&addr_len);

	// Si no se recibe algun mensaje del servidor despues de 2 seg se intenta
	// enviar el mensaje 3 veces mas.
	numeroIntentos = 1;
	while (numbytes2 == -1 && numeroIntentos <= 3){

		printf("Numero de intentos : %d\n",numeroIntentos);
		if ((numbytes=sendto(sockfd,mensaje,strlen(mensaje),0,
			(struct sockaddr *)&their_addr,sizeof(struct sockaddr))) == -1) { 
			perror("sendto"); 
			exit(2); 
		} 

		numbytes2 = recvfrom(sockfd,recibido,30,0, (struct sockaddr *)&their_addr,
			(socklen_t *)&addr_len);

		numeroIntentos = numeroIntentos + 1;
	}

	// Si hubo mas de tres intentos de conexion con el servidor entonces se 
	// aborta la ejecucion.
	if (numeroIntentos > 3){
		printf("El servidor se encuentra sin conexion. \n");
		exit(0);
	}
	else{

		recibido[numbytes2] = '\0'; 
		printf("Mensaje del servidor : %s\n",recibido);
		
	}
	printf("Numero de intentos final %d\n",numeroIntentos);

	// Se cierra el socket
	close(sockfd); 

	return 0;
}