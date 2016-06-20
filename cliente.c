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
	int identificador;
	int addr_len,numbytes; /* conteo de bytes a escribir */ 
	char *operacion;
	char mensaje[500];

	time_t tiempoActual;
    char* tiempoStr;

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

	sprintf(mensaje, "%d",identificador);
	strcat(mensaje,",");
	strcat(mensaje,operacion);

    /* Obtain current time. */
    tiempoActual = time(NULL);

    if (tiempoActual == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    tiempoStr = ctime(&tiempoActual);

    if (tiempoStr == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }

    strcat(mensaje,",");
    strcat(mensaje,tiempoStr);

	printf("mensaje :%s\n", mensaje);

	
	/* enviamos el mensaje */ 
	if ((numbytes=sendto(sockfd,mensaje,strlen(mensaje),0,
		(struct sockaddr *)&their_addr,sizeof(struct sockaddr))) == -1) { 
		perror("sendto"); 
		exit(2); 
	} 

	/* cierro socket */ 
	close(sockfd); 


	return 0;
}