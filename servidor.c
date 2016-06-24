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

int calcularCosto(char *identificador) {

	/*  Descripcion de la funcion:
			Esta funcion dado el nombre de un archivo lee todo su 
		contenido.Si el archivo no esta vacio se crea una lista enlazada 
		de elementos de tipo PREGUNTA, este contiene los siguientes 
		(componentes):
		 		-codigo: Entero que representa el codigo de una 
		 				 pregunta.
		 		-nivel:  Entero que representa el nivel de dificultad 
		 				 de una pregunta.
		 		-area:   Char que representa el area de conocimiento
		 				 a la que pertenece una pregunta.
		 		-pregunta: String que representa la pregunta
		 		-opcion1:  String que representa una posible respuesta 1
		 		-opcion2:  String que representa una posible respuesta 2
		 		-opcion3:  String que representa una posible respuesta 3
		 		-respuesta: Entero que representa cual es el numero
		 					de la opcion con la respuesta correcta.
		 		-siguiente: Apuntador al siguiente elemento de tipo 
		 					PREGUNTA
		 Si el archivo esta vacio la funcion solo creara un apuntador a 
		 null. 
		*/

	int horas;
	int minutos;
	int horasTotales;
	int minutosTotales;
	int len;
	time_t tiempoActual;
    struct tm *tiempoSalida;
	FILE *archivoS;

	archivoS = fopen(identificador,"r");
	len = fscanf(archivoS," %d:%d",&horas,&minutos);

	printf("Linea extraida %d : %d \n",horas,minutos);
	fclose(archivoS);

	// Se calcula el tiempo actual.
    tiempoActual = time(NULL);

    if (tiempoActual == ((time_t)-1))
    {
        printf("Error al obtener hora actual.\n");
        exit(0);
    }

    tiempoSalida = localtime( &tiempoActual);
    horasTotales =  tiempoSalida->tm_hour - horas;
    minutosTotales = tiempoSalida->tm_min - minutos;


    printf("Hora de salida: %.2d %.2d\n", 
        tiempoSalida->tm_hour, tiempoSalida->tm_min);
 

    if (minutosTotales < 0 ){
    	minutosTotales = 60 + minutosTotales;

    	if (horasTotales != 0){
    		horasTotales = horasTotales - 1 ;
    	}
    }

    if (horasTotales < 0){

    	horasTotales = horasTotales + 24;
    }

    printf("El tiempo del carro es: %.2d %.2d\n", 
        horasTotales, minutosTotales);

    if (horasTotales == 0) {

    	return 80;
    }
    else {

    	if (minutosTotales > 0 ) {

    		return 80*horasTotales + 30;
    	}
    	else {

    		return 80*horasTotales;
    	}
    }

}

int verificarID(char *archivoIdent,char *operacion){


	/*  Descripcion de la funcion:
			Esta funcion dado el nombre de un archivo lee todo su 
		contenido.Si el archivo no esta vacio se crea una lista enlazada 
		de elementos de tipo PREGUNTA, este contiene los siguientes 
		(componentes):
		 		-codigo: Entero que representa el codigo de una 
		 				 pregunta.
		 		-nivel:  Entero que representa el nivel de dificultad 
		 				 de una pregunta.
		 		-area:   Char que representa el area de conocimiento
		 				 a la que pertenece una pregunta.
		 		-pregunta: String que representa la pregunta
		 		-opcion1:  String que representa una posible respuesta 1
		 		-opcion2:  String que representa una posible respuesta 2
		 		-opcion3:  String que representa una posible respuesta 3
		 		-respuesta: Entero que representa cual es el numero
		 					de la opcion con la respuesta correcta.
		 		-siguiente: Apuntador al siguiente elemento de tipo 
		 					PREGUNTA
		 Si el archivo esta vacio la funcion solo creara un apuntador a 
		 null. 
		*/


	// Se verifica si el ID del vehiculo existe (Escribir una funcion!!!)

	// Si existe quiere decir que hay 
	if ( ( (access(archivoIdent,F_OK ) != -1) && \
		(strcmp(operacion,"e")==0) ) || ( (access(archivoIdent,F_OK ) == -1) && \
		(strcmp(operacion,"s")==0) ) ){

	    return 1;

	} 
	else {

		return 0;

	}



}

void escibirBitacoraSalida(char *bitacoraSalida,char *identificador,int montoApagar){

	FILE *archivoS;
	// Se abre el archivo
	archivoS = fopen(bitacoraSalida,"a");
	// Se escribe en el archivo
	fprintf(archivoS,"Vehiculo con ID %s pago: %d Bs.\n",identificador,montoApagar);
	// Se cierra el archivo.
	fclose(archivoS);
}

void crearArchivoVehiculo(char *archivoIdent,struct tm* tiempoEntrada){

	FILE *archivoCarros;
	// Se abre el archivo
	archivoCarros = fopen(archivoIdent,"w");
	// Se escribe en el archivo
	fprintf(archivoCarros,"%d:%d",tiempoEntrada->tm_hour, tiempoEntrada->tm_min);
	// Se cierra el archivo.
	fclose(archivoCarros);

}

int main(int argc, char *argv[])
{

	int sockfd; /* descriptor para el socket */ 
	int serverPort;
	int contador;
	int montoApagar;
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
	struct tm *tiempoEntrada;
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

		errorIdentificador = verificarID(archivoIdent,operacion);

		// Despues de verificar si existe un erro con el ID se incrementa
		// o se decrementa el numero de puestos disponibles.
		if (errorIdentificador == 0 ){

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

					montoApagar = calcularCosto(archivoIdent);
					escibirBitacoraSalida(bitacoraSalida,identificador,montoApagar);

					printf("Soy salida\n");
					strcat(mensajeCliente,"Usted debe pagar ");
					sprintf(mensajeCliente,"%d",montoApagar);
					strcat(mensajeCliente," Bs.");
				
					//remove(archivoIdent);


				}
				else if (strcmp(operacion,"e")==0){


					// Se calcula el tiempo actual.
				    tiempoActual = time(NULL);

				    if (tiempoActual == ((time_t)-1)){
				        printf("Error calculando el tiempo actual.\n");
				        exit(0);
				    }

				    // Se cambia el formato de la hora
				    fecha = ctime(&tiempoActual);

				    if (fecha == NULL){
				    	printf("Error cambiando el formato del tiempo.\n");
				    	exit(0);
			
				    }

				    tiempoEntrada = localtime(&tiempoActual);
					crearArchivoVehiculo(archivoIdent,tiempoEntrada);


					//escibirBitacoraEntrada(bitacoraEntrada,identificador,fecha);
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