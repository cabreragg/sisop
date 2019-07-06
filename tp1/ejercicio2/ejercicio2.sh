#!/bin/bash

#############################################################################
# ejercicio2.sh
# TP1 Ejercicio 2
# Integrantes:  Bouza Facundo   DNI 41332191
#               Cabrera Gonzalo DNI 37350600
#               Facal Ernesto   DNI 38983722
#               Marson Tomas    DNI 40808276
#               Viescas Julian  DNI 40258471
# Segunda reentrega.
#############################################################################

if [ "$1" == "-h" ] || [ "$1" == "--help" ] || [ "$1" == "-?" ]; then

echo -e "El objetivo general del script es filtar un archivo recibido por parametro (2do) por Nombre, DNI o CUIT y matchearlos con el primer parametro recibido, 
cuando encuentra la primera coincidencia imprime por unica vez la informacion de esa linea y corta el proceso.\n
Parametros: Dato Archivo Modo (-c -d -n).\n
-c: se buscara por CUIT.\n
-d: se buscara por DNI\n
-n: se buscara por nombre."
	
exit
fi

if [ $# != 3 ]; then
	echo "Error. No se han pasado tres parametros."
	exit
fi

if ! [ -f "$2" ]; then
	echo "El segundo parametro no fue un archivo."
	exit
fi

DATO=$1
MODO=$3
esstring='^[a-zA-Z[:space:]]+$'
esdni='^[0-9]{8}$'
escuit='^[0-9]{11}$'

if [ $MODO == '-d' ]; then
        if ! [[ $DATO =~ $esdni ]]; then
                echo "Error de datos. El parametro 1 debe ser un número de 8 digitos"
                exit
        fi
elif [ $MODO == '-c' ]; then
        if ! [[ $DATO =~ $escuit ]]; then
                echo "Error de datos. El parametro 1 debe ser un numero de 11 digitos"
                exit
        fi 
elif [ $MODO == '-n' ]; then
        if ! [[ $DATO =~ $esstring ]]; then
                echo "Error de datos. El parametro 1 debe ser un string"
                exit
        fi
else
        echo "Modo mal ingresado."
        exit
fi

awk -v dato="$DATO" 'BEGIN {
FIELDWIDTHS = "11 30 2 2 2 1 1 2"
d= ("'$MODO'"=="-n") ? "2" : "1"

CANT=0
}
match($d, dato) {
print "Nombre                         CUIT"
print "------------------------------ -----------"
print $2" "$1
print "Imp ganancias: "$3
print "Imp IVA: "$4
print "Monotributo: "$5
print "Integrante SOC: "$6
print "Empleador: "$7
print "Actividad monotributo: "$8
print "Cantidad de contribuyentes filtrados: "CANT
exit
}
{
CANT++
}' $2

#Fin de archivo
