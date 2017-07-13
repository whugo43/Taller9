all: servidor cliente

servidor: servidor.c 
	gcc -Wall -g $^ -o $@

cliente: cliente.c 
	gcc -Wall $^ -o $@

clean:
	rm -rf cliente servidor
