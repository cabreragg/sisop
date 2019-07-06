#!/bin/bash

#############################################################################
# Comparador.sh
# TP1 Ejercicio 4
# Integrantes: 	Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomas	DNI 40808276
#		Viescas Julian	DNI 40258471
# Segunda reentrega.
#############################################################################

function ayuda () {
cat <<-FIN

Acerca del script
-----------------
Este script controla la similitud de un archivo base contra los archivos contenidos
en un directorio. El script comparara el archivo pasado por parametro contra cada
uno de los archivos encontrados en una rama de directorios tambien pasado por
parametro, informando los archivos que tienen cierto porcentaje de igualdad.
Si no se informa el directorio, tomara el directorio actual.
El script recibira por parametro el archivo base, un porcentaje minimo de igualdad
y la ruta del directorio (opcional), en ese orden.
-----------------
Ejemplo: ./ejercicio4 archivo_base 60 directorio_a_comparar

FIN
}

if [ "$1" = "-h" ] || [ "$1" = "-help" ] || [ "$1" = "-?" ]; then
	ayuda
	exit
fi

if [ $# -ne 2 ] && [ $# -ne 3 ]; then
	echo -e "\nError de sintaxis. Recuerde que debe ingresar:\n
Primer parametro: archivo base o -h para ayuda\n
Segundo parametro: porcentaje de igualdad\n
Tercer parametro: directorio a comparar o sin parametro para comparar el actual\n"
	exit
fi

if ! [ -f "$1" ]; then
	echo "Debe ingresar una ruta de archivo valida como primer parametro"
	exit
fi

if [ $2 -gt "100" ] || [ "$2" -lt "0" ]; then
	echo "El parametro porcentual debe estar entre 0 y 100"
	exit
fi

if ! [ "$3" ]; then
	DIRECTORIO="./"
else
	DIRECTORIO="$3"
fi
declare -a archivos=($(find $DIRECTORIO -type f))
tam=${#archivos[@]}
i=0
while [ $i -lt $tam ]; do
	lineastotal=$(cat $1 | wc -l)
	lineasdistintas=$(sdiff -s $1 ${archivos[i]} | wc -l)
	total=$(( (lineastotal-lineasdistintas) * 100 / lineastotal ))
	if  [ $total -ge $2 ]; then
		echo "el archivo ${archivos[i]} cumple el porcentaje de igualdad" 
	else
		echo "el archivo ${archivos[i]} no cumple el porcentaje de igualdad"
	fi
	let i=$i+1
done

#Fin de archivo.
