/*
 Archivo: lib_servidor.c

 Nombres:
	Alejandra Cordero / Carnet: 12-10645
	Ricardo Mena  / Carnet: 12-10872

 Ultima modificacion: 24/06/2016

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


//----------------------------------------------------------------------------//

int calcularCosto(char *identificador) {


	/*  Descripcion de la funcion:

			Esta funcion dada la ruta de un archivo que contenga alguna fecha. 
		Calcula el tiempo actual, verifica cuanto tiempo ha transcurrido desde
		la fecha extraida del archivo y calcula un monto siguiendo las 
		siguientes consideraciones:
			- Las fraciones de hora vales 30
			- Cada hora vale 80.
			- La primera fraccion de hora vale 80

		Parametros de entrada:

			- identificador: Es la ruta del archivo que contiene la fecha.

		Parametros de salida:

			Monto a pagar segun el numero de horas que han trascurrido desde
		la fecha que se encuentra en el archivo.
	 
	*/

	// Declaracion de variables:
	int dia;
	int mes;
	int horas;
	int minutos;
	int diasTotales;
	int mesesTodales;
	int horasTotales;
	int minutosTotales;
	time_t tiempoActual;
    struct tm *tiempoSalida;
	FILE *archivoS;

	// Se abre el archivo y se lee la fecha
	archivoS = fopen(identificador,"r");
	fscanf(archivoS," %d %d %d:%d",&dia,&mes,&horas,&minutos);


	fclose(archivoS);

	// Se elimina el archivo
	remove(identificador);


	// Se calcula el tiempo actual.
    tiempoActual = time(NULL);

    if (tiempoActual == ((time_t)-1))
    {
        printf("Error al obtener hora actual.\n");
        exit(0);
    }

    // Se resta la fecha del tiempo actual con la fecha del archivo.
    tiempoSalida = localtime( &tiempoActual);
    diasTotales = tiempoSalida->tm_mday - dia;
    mesesTodales = tiempoSalida->tm_mon - mes;
    horasTotales =  tiempoSalida->tm_hour - horas;
    minutosTotales = tiempoSalida->tm_min - minutos;


    // Se rectifica la operacion de resta.
	if (mesesTodales != 0){

	    if (diasTotales < 0){

	    	if ( (mes == 0) | (mes == 2) | (mes == 4) | (mes == 6) | (mes == 7) | (mes == 9) | (mes == 11) ) {

	    		diasTotales = 31 + diasTotales;
	    	}

	    	else if (mes == 1){

	    		diasTotales = 28 + diasTotales;
	    	}

	    	else{
	    		diasTotales = 30 + diasTotales;
	    	}
	    }

	}

    if (minutosTotales < 0 ){
    	minutosTotales = 60 + minutosTotales;

    	if (horasTotales != 0){
    		horasTotales = horasTotales - 1 ;
    	}
    	if (diasTotales != 0 ){
    		diasTotales = diasTotales -1 ;
    	}
    }

    if (horasTotales < 0){

    	horasTotales = horasTotales + 24;
    }

	// Se calcula el monto a pagar
    if (horasTotales == 0 && diasTotales == 0) {

    	return 80;
    }
    else {

    	if (minutosTotales > 0 ) {

    		return 80*horasTotales + 24*80*diasTotales + 30;
    	}
    	else {

    		return 80*horasTotales + 24*80*diasTotales;
    	}
    }

}

//----------------------------------------------------------------------------//

int verificarID(char *archivoIdent,char *operacion){


	/*  Descripcion de la funcion:

			Esta funcion dada la ruta de un archivo y una operacion (e o s)
		verifica si esta o no creado un archivo.

			1 - Si la operacion es "e" y el archivo esta creado retornara 1 .
			2 - Si la operacion es "s" y el archivo no esta creado tambien retornara 1
			3 - Si el caso no es ninguno de los anteriores entonces retornara 0.

		Parametros de entrada:

			- archivoIdent : ruta del archivo a estudiar.
			- operacion : esta operacion puede ser "e" (entrada) o "s" (salida)

		Parametros de salida:

			Se retorna 1 (True) si se cumplen las condiciones 1 o 2 de la 
		descripcion de lo contrario retorna 0.
	 
	*/


	if ( ( (access(archivoIdent,F_OK ) != -1) && \
		(strcmp(operacion,"e")==0) ) || ( (access(archivoIdent,F_OK ) == -1) && \
		(strcmp(operacion,"s")==0) ) ){

	    return 1;

	} 
	else {

		return 0;

	}
}

//----------------------------------------------------------------------------//

void escibirBitacoraEntrada(char *bitacoraEntrada,char *identificador,char *fecha){


	/*  Descripcion de la funcion:

			Esta funcion dada la ruta de un archivo escribe en el un ID y una 
		fecha.

		Parametros de entrada:

			- bitacoraEntrada: Ruta del archivo en el cual se escribira
			- identificador : ID que se escribira en el archivo.
			- fecha: Fecha que se escribira en el archivo

		Parametros de salida:

			- Ninguno.
	 
	*/

	// Declaracion de variables:
	FILE *archivoE;

	// Se abre y se escribe en el archivo.
	archivoE = fopen(bitacoraEntrada,"a");
	fprintf(archivoE,"ID :%s ingreso en la fecha :%s",identificador,fecha);
	fclose(archivoE);

}

//----------------------------------------------------------------------------//


void escibirBitacoraSalida(char *bitacoraSalida,char *identificador,int montoApagar){


	/*  Descripcion de la funcion:

			Esta funcion dada la ruta de un archivo escribe en el un ID y un 
		monto.

		Parametros de entrada:

			- bitacoraSalida: Ruta del archivo en el cual se escribira
			- identificador : ID que se escribira en el archivo.
			- montoApagar: Monto que se escribira en el archivo

		Parametros de salida:

			- Ninguno.
	 
	*/
	// Declaracion de variables:
	FILE *archivoS;


	// Se abre el archivo
	archivoS = fopen(bitacoraSalida,"a");
	// Se escribe en el archivo
	fprintf(archivoS,"Vehiculo con ID %s pago: %d Bs.\n",identificador,montoApagar);
	// Se cierra el archivo.
	fclose(archivoS);
}

//----------------------------------------------------------------------------//

void crearArchivoVehiculo(char *archivoIdent,struct tm* tiempoEntrada){


	/*  Descripcion de la funcion:

			Dada el nombre  y una estructura de tiempo esta 
		funcion crea un archivo con el nombre dado y escribe sobre este la fecha 
		actual del sistema

		Parametros de entrada:

			- archivoIdent : Nombre del archivo a crear
			- tiempoEntrada : Estructura de tiempo que almacena el tiempo
							  actual del sistema.

		Parametros de salida:

			- Ninguno : 
	 
	*/

	// Declaracion de variables:
	FILE *archivoCarros;

	// Se abre el archivo
	archivoCarros = fopen(archivoIdent,"w");
	// Se escribe en el archivo
	fprintf(archivoCarros,"%d %d %d:%d",tiempoEntrada->tm_mday,tiempoEntrada->tm_mon,
					tiempoEntrada->tm_hour, tiempoEntrada->tm_min);

	
	// Se cierra el archivo.
	fclose(archivoCarros);

}

//----------------------------------------------------------------------------//

int contarVehiculosEstacionados(char *ruta){


	/*  Descripcion de la funcion:

			Dado el nombre de un directorio esta funcion cuenta todos los 
		archivos que se encuentran en el mismo exceptuando . y ..

			En el caso particular los archivos representan el numero se 
		vehivulos estacionados.
		
		Parametros de entrada:

			- ruta: Ruta del directorio del cual se contaran los archivos

		Parametros de salida:

			- Devuelve el numero de archivos del directorio dado.
	 
	*/

	// Declaracion de variables:
	int contadorArchivos = 0;	// Se inicializa la variable que contara 
								// el numero de carpetas.
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
