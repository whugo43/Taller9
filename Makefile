all: servidor cliente

servidor: servidor_multiproceso.c 
	gcc -Wall -g $^ -o $@

cliente: cliente_multiproceso.c 
	gcc -Wall $^ -o $@

clean:
	rm -rf cliente servidor archivo[0-9] archivo[0-9][0-9]
 
