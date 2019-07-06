#!/bin/bash

#############################################################################
# ejercicio1.sh
# TP1 Ejercicio 1
# Integrantes: 	Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomas	DNI 40808276
#		Viescas Julian	DNI 40258471
# Segunda reentrega.
#############################################################################

# a) Esta directiva especial se llama "Shebang" y le indica al loader con que interprete cargar este programa para poder ejecutarlo.

# b) Al script se le otorgo permisos de ejecucion con el comando 'chmod' y el nivel de permisos deseados (para este archivo 'chmod 775 ejercicio1.sh').

# c) El objetivo general del script es filtar un archivo recibido por parametro (2do) por los 11 primeros caracteres de cada linea (CUIT) y matchearlos con el primer
# parametro recibido, cuando encuentra la primera coincidencia imprime por unica vez los 41 primeros caracteres de esa linea (Nombre y CUIT) y corta el proceso..

# e) AWK es una herramienta para procesar texto, parsea texto (lineas de un archivo, output de otro comando) y lo divide por ciertos limitadores 
# (puede ser un caracter en especial, cantidad de caracteres o espacios en su defecto). Una vez que lo tiene dividido en campos, se pueden buscar patrones y operar
# con ese texto cuando se encuentren o no esos patrones definidos.

# f) Para procesar varios archivos con el script hay varias formas, una podria ser concatenando el contenido de varios
# archivos en uno solo y ejecutando el script con el archivo resultante:
# cat file1 file2 >> file3 ; ./ejercicio1.sh 12345678901 file3
# Otra forma sería ejecutar el script varias veces, una por cada archivo que se desee procesar:
# for f in {file1, file2, file3}; do ./ejercicio1.sh 12345678901 $f; done

if [ $# != 2 ]; then
	echo "Error en la cantidad de parametros..."
	exit
fi
if ! [ -f "$2" ]; then
	echo "El segundo parametro debe ser un archivo."
	exit
fi

cuit=$1

awk 'BEGIN {
	FIELDWIDTHS = "11 30"
}

$1 == '$cuit' {

print "Nombre			       CUIT"
print "------------------------------ ------------"
print $2" "$1

exit
}' $2   

#EOF
