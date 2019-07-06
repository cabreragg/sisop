#!/bin/bash

#############################################################################
# ejercicio6.sh
# TP1 Ejercicio 6
# Integrantes: 	Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomas	DNI 40808276
#		Viescas Julian	DNI 40258471
# Segunda reentrega.
#############################################################################

if [ "$1" == '-h' ] || [ "$1" == '--help' ] || [ "$1" == '-?' ]; then
	cat <<-FIN
	El script renombra masivamente los archivos de un directorio.
	El script recibira por parametro el directorio a analizar, el patron a buscar y el texto de reemplazo.
	FIN
	exit
fi

if [ $# != 3 ]; then
	echo "Error en la cantidad de parametros."
	exit
fi

if ! [ -d "$1" ]; then
	echo "El primer parametro debe ser un directorio"
	exit
fi

ARCHIVOS=($(ls "$1"))

for ((i=0; i<${#ARCHIVOS[@]}; i++))
do
    	OLDNAME=${ARCHIVOS[$i]}
    	first=${ARCHIVOS[$i]}
	second="$2"
	NEWNAME="${first//$second/$3}"
	if [ $NEWNAME != $OLDNAME ]; then
		mv "$1/$OLDNAME" "$1/$NEWNAME"
	fi
done

ls "$1"

#Fin de archivo
