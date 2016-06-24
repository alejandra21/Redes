/*
*
* Archivo: lib_servidor.c
*
* Nombres:
*	Alejandra Cordero / Carnet: 12-10645
*	Ricardo Mena  / Carnet: 12-10872
*
* Descripcion: Encabezado del archivo
* lib_servidor.c
*
*
*/

#ifndef FUNCIONES_H
#define FUNCIONES_H

int calcularCosto(char *identificador);
int verificarID(char *archivoIdent,char *operacion);
void escibirBitacoraEntrada(char *bitacoraEntrada,char *identificador,char *fecha);
void escibirBitacoraSalida(char *bitacoraSalida,char *identificador,int montoApagar);
void crearArchivoVehiculo(char *archivoIdent,struct tm* tiempoEntrada);
int contarVehiculosEstacionados(char *ruta);

#endif