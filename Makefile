all: servidorT clienteT

servidorT: servidor_taller.c 
	gcc -Wall -g $^ -o $@

clienteT: cliente_taller.c 
	gcc -Wall $^ -o $@

clean:
	rm -rf clienteT servidorT
