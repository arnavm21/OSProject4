all: memorymanager

scheduler: memorymanager.c
	gcc -o memorymanager memorymanager.c


clean: memorymanager.c
	rm -f memorymanager