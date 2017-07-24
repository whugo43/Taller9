#!/bin/bash

for value in {1..1000}
do
	for  res in {0..49}; do
		echo ""
		echo "archivos_aleatorios/archivo$res"
		./cliente 127.0.0.1 33332 archivos_aleatorios/archivo$res archivo$res
	done	
done
