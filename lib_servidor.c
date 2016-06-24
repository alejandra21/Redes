/*
 Archivo: lib_servidor.c

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
#include <sys/types.h> 
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h> 
#include <fcntl.h>
#include <time.h>


int calcularCosto(char *identificador) {


	/*  Descripcion de la funcion:

		Parametros de entrada:

		Parametros de salida:
	 
	*/

	int horas;
	int minutos;
	int horasTotales;
	int minutosTotales;
	time_t tiempoActual;
    struct tm *tiempoSalida;
	FILE *archivoS;

	archivoS = fopen(identificador,"r");
	fscanf(archivoS," %d:%d",&horas,&minutos);

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

//------------------------------------------------------------//

int verificarID(char *archivoIdent,char *operacion){


	/*  Descripcion de la funcion:

		Parametros de entrada:

		Parametros de salida:
	 
	*/

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

//------------------------------------------------------------//

void escibirBitacoraEntrada(char *bitacoraEntrada,char *identificador,char *fecha){


	/*  Descripcion de la funcion:

		Parametros de entrada:

		Parametros de salida:
	 
	*/

	FILE *archivoE;
	archivoE = fopen(bitacoraEntrada,"a");
	fprintf(archivoE,"ID :%s ingreso en la fecha :%s",identificador,fecha);
	fclose(archivoE);

}

//------------------------------------------------------------//


void escibirBitacoraSalida(char *bitacoraSalida,char *identificador,int montoApagar){


	/*  Descripcion de la funcion:

		Parametros de entrada:

		Parametros de salida:
	 
	*/

	FILE *archivoS;
	// Se abre el archivo
	archivoS = fopen(bitacoraSalida,"a");
	// Se escribe en el archivo
	fprintf(archivoS,"Vehiculo con ID %s pago: %d Bs.\n",identificador,montoApagar);
	// Se cierra el archivo.
	fclose(archivoS);
}

//------------------------------------------------------------//

void crearArchivoVehiculo(char *archivoIdent,struct tm* tiempoEntrada){


	/*  Descripcion de la funcion:

		Parametros de entrada:

		Parametros de salida:
	 
	*/

	FILE *archivoCarros;
	// Se abre el archivo
	archivoCarros = fopen(archivoIdent,"w");
	// Se escribe en el archivo
	fprintf(archivoCarros,"%d:%d",tiempoEntrada->tm_hour, tiempoEntrada->tm_min);
	// Se cierra el archivo.
	fclose(archivoCarros);

}

//------------------------------------------------------------//

int contarVehiculosEstacionados(char *ruta){


	/*  Descripcion de la funcion:

		Parametros de entrada:

		Parametros de salida:
	 
	*/

	// Declaracion de variables:
	int contadorArchivos = 0;	// Se inicializa la variable que contara el numero de carpetas.
	DIR *dirp;					// Variable que se utilizara para abrir el directorio.
	struct dirent *direntp;		// Estructura que permite leer los archivos.
	
	// Se verifica si se puede abrir el directorio:
	if ((dirp = opendir(ruta)) == NULL) {

		//fprintf(stderr,"No se puede abrir el directorio %s\n");
		strerror(errno);
		exit(1);

	}

	// Se cuenta el numero de archivos del directorio ./carros:
	while ((direntp = readdir(dirp)) != NULL ) {

		contadorArchivos++;
	}

	return contadorArchivos - 2;

}