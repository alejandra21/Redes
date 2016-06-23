#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> 
#define BUFFER_LEN 1024 
#define PUESTOS_DISP 200 

int main(int argc, char *argv[])
{

	int sockfd; /* descriptor para el socket */ 
	int serverPort;
	int contador;
	int puestosDisponibles = 200;
	char *bitacoraEntrada;
	char *bitacoraSalida;
	char *token;
	char *identificador;
	char *operacion;
	char *fecha; 
	char *recibido;
	char archivoIdent[100];
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

	pid_t childpid;	

	mkdir("./carros", 0700);

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


	// Declaracion de variables:
	int contadorArchivos = 0;	// Se inicializa la variable que contara el numero de carpetas.
	DIR *dirp;					// Variable que se utilizara para abrir el directorio.
	struct dirent *direntp;		// Estructura que permite leer los archivos.
	
	// Se verifica si se puede abrir el directorio:
	if ((dirp = opendir("./carros")) == NULL) {

		//fprintf(stderr,"No se puede abrir el directorio %s\n");
		strerror(errno);
		exit(1);

	}

	// Se cuenta el numero de archivos del directorio:
	while ((direntp = readdir(dirp)) != NULL ) {

		contadorArchivos++;
	}

	puestosDisponibles = puestosDisponibles - contadorArchivos;

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


		// Falta revisar el id
		if (strcmp(operacion,"s")==0){

			puestosDisponibles = puestosDisponibles + 1;

		}

		else if (strcmp(operacion,"e")==0){

			puestosDisponibles = puestosDisponibles - 1;

		}

		childpid = fork();
		if (childpid == 0) {

			printf("Soy el hijo \n");
			printf("Puestos disponibles:  %d\n",puestosDisponibles);
			printf("operacion %s\n",operacion);
			if (strcmp(operacion,"s")==0){

				FILE *archivoS;

				archivoS = fopen(bitacoraSalida,"a");
				fprintf(archivoS,"%s %s",identificador,fecha);

				// Se cierra el archivo.
				fclose(archivoS);
				printf("Soy salida\n");

			}
			else if (strcmp(operacion,"e")==0){

				// Se crea el archivo de los identificadores de los carros.
				memset(archivoIdent, 0, sizeof archivoIdent);
				strcat(archivoIdent,"./carros/");
				strcat(archivoIdent,identificador);


				FILE *archivoCarros;
				archivoCarros = fopen(archivoIdent,"w");
				fprintf(archivoCarros,"%s",fecha);
				fclose(archivoCarros);

				FILE *archivoE;
				archivoE = fopen(bitacoraEntrada,"a");
				fprintf(archivoE,"%s %s",identificador,fecha);

				// Se cierra el archivo.
				printf("Soy entrada\n");
				fclose(archivoE);

				printf("Archivos %d\n",puestosDisponibles);

				if (puestosDisponibles == 0){

					recibido = "No hay chance";
					remove(archivoIdent);
				}

				else {

					recibido = "Hay chance";

				}

				if ((numbytes2=sendto(sockfd,recibido,strlen(recibido),0,
					(struct sockaddr *)&their_addr, 
					sizeof(struct sockaddr))) == -1) { 
					perror("sendto"); 
					exit(2); 
				} 

			}

/*			buf[numbytes] = '\0'; 
			printf("el paquete contiene: %s\n", buf);
*/
			exit(0);


		}

		else{

			waitpid(-1,NULL,WNOHANG);
		}
	}

	/* cerramos descriptor del socket */ 
	close(sockfd); 
 
	return 0;
}