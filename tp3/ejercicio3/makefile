DIR=./src

ejercicio3	:	main.o
				gcc	-o	ejercicio3	main.o
main.o	:	$(DIR)/main.c	$(DIR)/constants.h	$(DIR)/structs.h
			gcc	-c	$(DIR)/main.c
clean	:	
			rm -rf -f *.o ejercicio3 ./output