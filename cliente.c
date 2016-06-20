#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 

int main(int argc, char *argv[])
{
	int serverPort;
	int sockfd;
	int identificador;
	int addr_len,numbytes; /* conteo de bytes a escribir */ 
	char *operacion;

	struct sockaddr_in their_addr; /* almacenara la direccion IP y numero de puerto del servidor */ 
	struct hostent *direccionDestino; 


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
					identificador = atoi(argv[i+1]);

				}

				else{

					printf("Los argumentos no fueron pasados de forma \
						    correcta.\n");
					exit(0);
				}
			}

		}


	}

	/* Creamos el socket */ 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
		perror("socket"); 
		exit(2); 
	} 


	/* a donde mandar */ 
	their_addr.sin_family = AF_INET; /* usa host byte order */ 
	their_addr.sin_port = htons(serverPort); /* usa network byte order */ 
	their_addr.sin_addr = *((struct in_addr *)direccionDestino->h_addr); 
	bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */ 

	char hola[100];
	scanf("%[^\n]s :",hola);
	
	/* enviamos el mensaje */ 
	if ((numbytes=sendto(sockfd,hola,strlen(hola),0,
		(struct sockaddr *)&their_addr,sizeof(struct sockaddr))) == -1) { 
		perror("sendto"); 
		exit(2); 
	} 

	/* cierro socket */ 
	close(sockfd); 


	return 0;
}