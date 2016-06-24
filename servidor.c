/*#
# Archivo: Makefile
#
# Nombres:
#	Alejandra Cordero / Carnet: 12-10645
#	Ricardo Mena  / Carnet: 12-10872
#
# Ultima modificacion: 09/05/2015
#
#*/


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
#include <fcntl.h>
#include <time.h>
#define BUFFER_LEN 1024 
#define PUESTOS_DISP 200 

int main(int argc, char *argv[])
{

	int sockfd; /* descriptor para el socket */ 
	int serverPort;
	int contador;
	int puestosDisponibles = 200;
	int errorIdentificador;
	time_t tiempoActual;
	char* fecha;
	char *bitacoraEntrada;
	char *bitacoraSalida;
	char *token;
	char *identificador;
	char *operacion;
	char *recibido;
	char mensajeCliente[100]; // Debe ser dinamico
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

	// Se cuenta el numero de archivos del directorio ./carros:
	while ((direntp = readdir(dirp)) != NULL ) {

		contadorArchivos++;
	}

	puestosDisponibles = puestosDisponibles - contadorArchivos;

	while(1) {


		if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0,
			(struct sockaddr *)&their_addr,
			(socklen_t *)&addr_len)) == -1) { 

			perror("recvfrom"); 
			exit(3); 
		}

		errorIdentificador = 0;

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

			printf("%s\n",token);
			token = strtok(NULL,",");
			contador = contador +1 ;

		}

		// Se crea la ruta del archivo que contendra informacion de la fecha de 
		// llegada del nuevo carro.
		memset(archivoIdent, 0, sizeof archivoIdent);
		strcat(archivoIdent,"./carros/");
		strcat(archivoIdent,identificador);

		// Se verifica si el ID del vehiculo existe (Escribir una funcion!!!)

		// Si existe quiere decir que hay 
		if ( ( (access(archivoIdent,F_OK ) != -1) && \
			(strcmp(operacion,"e")==0) ) || ( (access(archivoIdent,F_OK ) == -1) && \
			(strcmp(operacion,"s")==0) ) ){

		    errorIdentificador = 1;

		} 
		else {

			if (strcmp(operacion,"s")==0){

				puestosDisponibles = puestosDisponibles + 1;

			}

			else if (strcmp(operacion,"e")==0){

				puestosDisponibles = puestosDisponibles - 1;

			}

		}


		childpid = fork();
		if (childpid == 0) {

			printf("Soy el hijo \n");
			printf("Puestos disponibles:  %d\n",puestosDisponibles);
			printf("operacion %s\n",operacion);

			if (errorIdentificador == 0){

				memset(mensajeCliente, 0, sizeof mensajeCliente);
				if (strcmp(operacion,"s")==0){

					// Escribir en una funcion!!!
					FILE *archivoS;
	/*				int line;
					int len;
					archivoS = fopen(archivoIdent,"r");

					len = fscanf(archivoS," %d",&line);
					printf("Linea %d\n",len);

					printf("Linea extraida %d \n",line);
					fclose(archivoS);*/


					archivoS = fopen(bitacoraSalida,"a");
					fprintf(archivoS,"%s ",identificador);
			
					// Se cierra el archivo.
					fclose(archivoS);
					printf("Soy salida\n");
					strcat(mensajeCliente,"Usted debe pagar ");
					strcat(mensajeCliente,"30 Bs.");
				
					remove(archivoIdent);


				}
				else if (strcmp(operacion,"e")==0){


					// Se calcula el tiempo actual.
				    tiempoActual = time(NULL);

				    if (tiempoActual == ((time_t)-1))
				    {
				        (void) fprintf(stderr, "Failure to obtain the current time.\n");
				        exit(EXIT_FAILURE);
				    }

				    printf("Tiempo actual %ld \n",tiempoActual);
				    // Se cambia el formato de la hora
				    fecha = ctime(&tiempoActual);

				    if (fecha == NULL)
				    {
				        (void) fprintf(stderr, "Failure to convert the current time.\n");
				        exit(EXIT_FAILURE);
				    }

				    printf(" fecha %s\n",fecha);
					// Escribir en una funcion!!!
					FILE *archivoCarros;
					archivoCarros = fopen(archivoIdent,"w");
					fprintf(archivoCarros,"%ld",tiempoActual);
					fclose(archivoCarros);

					FILE *archivoE;
					archivoE = fopen(bitacoraEntrada,"a");
					fprintf(archivoE,"%s %s",identificador,fecha);

					strcat(mensajeCliente,identificador);
					strcat(mensajeCliente," ");
					strcat(mensajeCliente,fecha);

					// Se cierra el archivo.
					printf("Soy mensajeCliente %s \n",mensajeCliente);
					fclose(archivoE);

					printf("Archivos %d\n",puestosDisponibles);

					if (puestosDisponibles == 0){
						strcat(mensajeCliente,"No hay chance");
					}

					else {

						strcat(mensajeCliente,"Hay chance");


					}


				}

			}

			else {

				strcat(mensajeCliente,"Hay un error con su ID.");

			}

			if ((numbytes2=sendto(sockfd,mensajeCliente,strlen(mensajeCliente),0,
				(struct sockaddr *)&their_addr, 
				sizeof(struct sockaddr))) == -1) { 
				perror("sendto"); 
				exit(2); 
			} 

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