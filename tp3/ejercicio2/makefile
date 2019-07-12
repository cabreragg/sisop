DIR=./src

ejercicio2 :	main.o	queue.o
	gcc -o ejercicio2	main.o	queue.o	-pthread
	
main.o :	$(DIR)/main.c	$(DIR)/queue.h	$(DIR)/structs.h
	gcc -g3	-c $(DIR)/main.c -L pthread

queue.o : $(DIR)/queue.c	$(DIR)/queue.h	$(DIR)/structs.h
	gcc -g3	-c $(DIR)/queue.c

clean :
	rm	-rf	*.o	ejercicio2	./output	
