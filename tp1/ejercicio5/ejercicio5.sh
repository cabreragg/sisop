#!/bin/bash

#############################################################################
# ejercicio5.sh
# TP1 Ejercicio 5
# Integrantes: 	Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomas	DNI 40808276
#		Viescas Julian	DNI 40258471
# Segunda reentrega.
#############################################################################

if [ "$1" == '-h' ] || [ "$1" == '--help' ] || [ "$1" == '-?' ]; then
	cat <<-FIN
	El script genera un backup comprimido de una ruta especificada.
	El script recibira los siguientes parametros:
	• Ruta de origen.
	• -t para backupear todos los archivos o
	• -x [extension] para backupear los archivos con dicha extension.Los parametros deben ser Ruta de origen y -t o -x
	FIN
	exit
fi

if [ $# != 2 ] && [ $# != 3 ]; then
	echo "La cantidad de parametros no es correcta..."
	exit
fi

DIA=`date +"%d-%m-%Y_"`
HORA=`date +"%H-%M-%S"`
DIRECTORIO="$1"

if [ $2 == '-t' ]; then
	NOMBRE="$DIA$HORA"
	EXT="[0-9].tar"
elif [ $2 == '-x' ]; then
	NOMBRE="$DIA$HORA$3"
	DIRECTORIO+="/*$3"
	EXT="[0-9]$3.tar"
else
	echo "Error en el segundo parametro"
	exit
fi

OUTPUT="$(tar cvfP $NOMBRE.tar $DIRECTORIO)"
ARRAY=($(ls -tr|grep $EXT))

if [ ${#ARRAY[@]} -gt 5 ]; then
	rm "${ARRAY[0]}"
fi

touch "$NOMBRE.log"
chmod 777 "$NOMBRE.log"

USUARIO=$(whoami)

echo "USUARIO: $USUARIO" > "$NOMBRE.log"
echo "Dia de generacion: $DIA" >> "$NOMBRE.log"
echo "Hora: $HORA" >> "$NOMBRE.log"
echo "$OUTPUT" >> "$NOMBRE.log"
exit

#Fin de archivo
