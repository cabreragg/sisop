#!/bin/bash

#############################################################################
# ejercicio3.sh
# TP1 - Ejercicio 3
# Integrantes: 	Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomas	DNI 40808276
#		Viescas Julian	DNI 40258471
# Segunda reentrega.
#############################################################################

if [ "$1" == '-h' ] || [ "$1" == '--help' ] || [ "$1" == '-?' ]; then
	cat <<-FIN
	SISOP - TP1 - EJ3 - Bingo bash script (2019 Apr 31)

	El proposito de este script es controlar los cartones de bingo de una agencia de loteria. Dichos cartones se encuentran guardados en un archivo
	de texto que se recibira por parametro. En cada linea del archivo se encontrara el numero de carton y 15 numeros separados por tabs que representan
	los numeros que participaran del sorteo para cada carton.
	El script no comenzara a procesar los cartones hasta no recibir la signal 'SIGUSR1' (ejecutar cmd: kill -10 PID del proceso).
	Controlara que cartones son los primeros en completar 1 linea (cada 5 numeros de los 15 que conforman el carton).
	El carton que complete las 3 lineas sera declarado el ganador del Bingo.
	El proceso termina una vez se encuentra el ganador del Bingo o bien cuando se sorteen todas las bolillas del 1 al 99 sin encontrar un ganador.
	
	usage: ./ejercicio3.sh [file]	Process cards file
	
	example usage:	./ejercicio3.sh cartones.txt
		or:		/home/user/ejercicio3.sh /home/user/cartones.txt
		or:		"/home/user/white space/ejercicio3.sh" "/home/user/white space/cartones.txt"	

	Arguments:
		-h  or  --help  or  -?	Print Help (this message) and exit
	FIN
	exit
fi

trap '' 2
trap 'startProcess' SIGUSR1
trap 'rm -f /tmp/.cards.txt' 0 2 3 15

startProcess(){
	echo "Empezando a procesar cartones del archivo: "$file"..."
	
	createControlCards

	loopLogic
	
	echo "Se sortearon $ballsDrawn bolillas."
	echo "Terminando proceso..."
	
	exit
}

loopLogic(){
	
	while [ $bingo == false ]
	do
		getRandom
		
		if [ $ballsDrawn -lt 5 ]; then
			continue
		fi

		checkForWinner
	done
}

getRandom(){
	i=$(( $RANDOM % 100 ))
        
        while [ ${numbers[ $i ]} ] && [ $ballsDrawn -lt 100 ]
        do
        	i=$(( $RANDOM % 100 ))
        done
	
	if [ $ballsDrawn -eq 100 ] && [ $bingo == false ]; then
		echo "Se sortearon todas las bolillas y no se encontro un ganador, verificar cartones..."
		exit
	fi

        numbers[$i]=true

	((ballsDrawn++))

	echo "Numero sorteado: $i."
 	
	controlCards $i
}

controlCards(){
	n="$1"

	awk 'BEGIN {
		OFS = "\t" 
	}
	($1 ~ /^[0-9]+$/) && (NR==FNR) {
		for (x=2; x <= NF; x++){
			if ($x == '$n')
				map[$1] = (x-1)
		}
		next
	}

	{
		if (NR != FNR){	 
			if ($20 in map){
				$(map[$20]) = 1
		
				for (a=1; a <= 15; a++){
                	        	l += $a

                        		if (a % 5 == 0){ 
                                		$(a/5 + 15) = l
                         			l = 0
					}
				}

				$19 = $16 + $17 + $18
			}
			print $0	
		}
	}' "$file" "/tmp/.cards.txt" > "/tmp/.temp" && mv "/tmp/.temp" "/tmp/.cards.txt"
}

createControlCards(){
	awk '$1 ~ /^[0-9]+$/ {
		for (x=1; x <= 19; x++)
			printf "%d\t", 0
		
		printf "%s\n", $1

	}' "$file" > "/tmp/.cards.txt"
}

checkForWinner(){
	while read input;
	do
        	IFS=$'\t' read -r -a arr <<< "$input"

        	if [ $line != true ]; then
                	for i in {15,16,17}
               		do
                        	if [ "${arr[i]}" -eq 5 ]; then
                                	echo 'El carton No.: ' "${arr[19]}" ' completo la linea: ' $(($i - 14))'.'
                                	line=true
                        	fi
                	done
        	fi
		
		if [ $bingo != true ] && [ "${arr[18]}" -eq 15 ]; then
                	echo '--- BINGO! --- El carton No.: ' "${arr[19]}" ' completo todas las lineas.'
                	bingo=true
        	fi

	done < "/tmp/.cards.txt"
}

if [ $# != 1 ]; then
	echo "No se recibio ningun archivo para procesar."
	exit
fi

if ! [ -f  "$1" ]; then
	echo "El parametro deber ser uno de los tipos de archivos soportados..."
	exit
fi

file="$1"

bingo=false
line=false

ballsDrawn=0

echo "Para comenzar el proceso mandar la signal SIGUSR1: PID $BASHPID..."

while true;
do
	sleep 2
done
#EOF
