/*
 Archivo: servidor.c

 Nombres:
	Alejandra Cordero / Carnet: 12-10645
	Ricardo Mena  / Carnet: 12-10872

 Ultima modificacion: 25/06/2016

*/

// Directivas de Preprocesador:
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
#include "lib_servidor.h"

int main(int argc, char *argv[])
{
	// Declaracion de variables

	int sockfd;  // Descriptor del socket
	int serverPort;
	int montoApagar;
	int contadorArchivos;
	int errorIdentificador;
	int addr_len, numbytes, numbytes2; 
	int tiempo;
	int puestosDisponibles = 200;

	// Tipos de datos
	uint16_t entrada = 0;
	uint16_t salida = 1;
/*	uint16_t ack = 2;
	uint16_t rr =3;*/
	uint16_t entradaCorrecta = 4;
	uint16_t sinPuesto = 5;
    uint16_t pagoCorrecto = 6;
    uint16_t errorID = 7;
	uint16_t operacion;
	uint32_t identificador;
	pid_t childpid;	
	time_t tiempoActual;
	char *fecha;
	char *op;
	char *bitacoraEntrada;
	char *bitacoraSalida;
	char ident[20];
	char *ruta = "./vehiculos";
	char archivoIdent[100];	  
	struct sockaddr_in my_addr;    //direccion IP y numero de puerto local  
	struct sockaddr_in their_addr; //direccion IP y numero de puerto del cliente 
	struct tm *tiempoEntrada;

    struct message{
        uint16_t operacion;
        uint32_t id;
        uint32_t datos;
    } mensaje;

	// Se verifican los argumentos
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

	// Se crea una carpeta para guardar la informacion de la hora de los
	// vehiculos que entran.
	printf("El servidor esta iniciando...\n");
	mkdir(ruta, 0700);

	/* se crea el socket */ 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { 
		perror("socket"); 
		exit(1); 
 	} 

 	// Se establece la estructura my_addr para luego llamar a bind() 
	my_addr.sin_family = AF_INET; 
	my_addr.sin_port = htons(serverPort); 
	my_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(my_addr.sin_zero), 8); 
					
	// Se le da un nombre al socket (se lo asocia al puerto e IPs)
	
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) { 
		perror("bind"); 
		exit(2); 
	}

	/* Se reciben los datos  */ 
	addr_len = sizeof(struct sockaddr); 

	// Se cuenta el numero de vehiculo que ya estan estacionados.
	contadorArchivos = contarVehiculosEstacionados(ruta);

	// Se calcula el numero de puestos disponibles.
	puestosDisponibles = puestosDisponibles - contadorArchivos;

	while(1) {

		if ((numbytes=recvfrom(sockfd, &mensaje, sizeof(mensaje), 0,
			(struct sockaddr *)&their_addr,
			(socklen_t *)&addr_len)) == -1) { 

			perror("Problemas en funcion recvfrom:"); 
			exit(3); 
		}

        operacion = ntohs(mensaje.operacion);
        identificador = ntohl(mensaje.id);

		// Se crea la ruta del archivo que contendra informacion de la fecha de 
		// llegada del nuevo carro.
		memset(archivoIdent, 0, sizeof archivoIdent);
		sprintf(ident, "%d", identificador);
		strcat(archivoIdent,"./vehiculos/");
		strcat(archivoIdent,ident);

        if (operacion == entrada){
            op = "e";
        }
        else {
            op = "s";
        }

		errorIdentificador = verificarID(archivoIdent,op);

		// Despues de verificar si existe un erro con el ID se incrementa
		// o se decrementa el numero de puestos disponibles.
		if (errorIdentificador == 0 ){

			if (operacion== entrada){

				puestosDisponibles = puestosDisponibles - 1;

			}

			else if (operacion== salida){

				puestosDisponibles = puestosDisponibles + 1;

			}

		}

		// Se crea un hijo
		childpid = fork();

		// Codigo que ejecuta el proceso hijo
		if (childpid == 0) {

			// Si no hay algun error con el ID del vehiculo
			if (errorIdentificador == 0){

				if (operacion == salida){

					montoApagar = calcularCosto(archivoIdent);
					escibirBitacoraSalida(bitacoraSalida,ident,montoApagar);

					mensaje.operacion = pagoCorrecto;
					mensaje.datos = montoApagar;
					

				}
				else if (operacion == entrada){


					// Se verifica si hay puestos disponibles
					if (puestosDisponibles == 0){

						mensaje.operacion = sinPuesto;

					}
					else {

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
					    // Se crea el archivo del vehiculo
						crearArchivoVehiculo(archivoIdent,tiempoEntrada);

						// Se escribe en la bitacora de entrada
						escibirBitacoraEntrada(bitacoraEntrada,ident,fecha);

						// Se arma el mensaje del cliente
					    tiempo = (int)tiempoActual;
						mensaje.operacion = entradaCorrecta;
						mensaje.id = identificador;
						mensaje.datos = tiempo;

					}

				}

			}

			else {
				// Se envia un mensaje de error
                mensaje.operacion = errorID;

			}

            mensaje.operacion = htons(mensaje.operacion);
            mensaje.id =  htonl(mensaje.id);
            mensaje.datos =  htonl(mensaje.datos);

			if ((numbytes2=sendto(sockfd,&mensaje,sizeof(mensaje),0,
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

	// Se cierra el descriptor del socket 
	close(sockfd); 
 
	return 0;
}
