all: servidor cliente

servidor: servidor_taller.c 
	gcc -Wall -g $^ -o $@

cliente: cliente_taller.c 
	gcc -Wall $^ -o $@

clean:
	rm -rf cliente servidor
