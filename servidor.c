#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#define BUFFER_LEN 1024 
#define PUESTOS_DISP 200 

int main(int argc, char *argv[])
{

	int sockfd; /* descriptor para el socket */ 
	int serverPort;
	int contador;
	char *bitacoraEntrada;
	char *bitacoraSalida;
	char *recibido;
	char *token;
	char *identificador;
	char *operacion;
	char *fecha; 
	struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */ 
	struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
	int addr_len, numbytes, numbytes2; 
	
	char buf[BUFFER_LEN]; /* Buffer de recepción */ 


	/* code */
	if (argc != 7){

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

				if ( strcmp(argv[i],"-l") == 0) {
					serverPort = atoi(argv[i+1]);

				}
				else if ( strcmp(argv[i],"-i") == 0){
					bitacoraEntrada = argv[i+1];

				}
				else if ( strcmp(argv[i],"-o") == 0){
					bitacoraSalida = argv[i+1];

				}

				else{

					printf("Los argumentos no fueron pasados de forma correcta.\n");
					exit(0);
				}
			}

		}


	}

	/* se crea el socket */ 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
		perror("socket"); 
		exit(1); 
 	} 

 	/* Se establece la estructura my_addr para luego llamar a bind() */ 
	my_addr.sin_family = AF_INET; /* usa host byte order */ 
	my_addr.sin_port = htons(serverPort); /* usa network byte order */ 
	my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */ 
	bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */ 
					
	/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */ 
	printf("Asignado direccion al socket ....\n"); 
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) { 
		perror("bind"); 
		exit(2); 
	}

	/* Se reciben los datos (directamente, UDP no necesita conexión) */ 
	addr_len = sizeof(struct sockaddr); 

	while(1) {

		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
			(socklen_t *)&addr_len)) == -1) { 
			perror("recvfrom"); 
			exit(3); 
		} 

		// Se divide el mensaje en tokens
		buf[numbytes] = '\0'; 
		token = strtok(buf,",");
		contador = 0;
		while( token != NULL )  {

			// Se tiene que poner case
			if (contador == 0){
				identificador = token;
			}
			else if (contador == 1){

				operacion = token;
			}

			else {
				fecha = token;
			}
			printf("%s\n",token);
			token = strtok(NULL,",");
			contador = contador +1 ;

		}

		if (strcmp(operacion,"s")==0){

			FILE *archivoS;

			archivoS = fopen(bitacoraSalida,"a");
			fprintf(archivoS,"%s %s",identificador,fecha);

			// Se cierra el archivo.
			fclose(archivoS);
			printf("Soy salida\n");

		}

		else if (strcmp(operacion,"e")==0){

			FILE *archivoE;

			archivoE = fopen(bitacoraEntrada,"a");
			fprintf(archivoE,"%s %s",identificador,fecha);

			// Se cierra el archivo.
			printf("Soy entrada\n");
			fclose(archivoE);

		}

		buf[numbytes] = '\0'; 
		printf("el paquete contiene: %s\n", buf); 

	}

	/* cerramos descriptor del socket */ 
	close(sockfd); 
 
	return 0;
}