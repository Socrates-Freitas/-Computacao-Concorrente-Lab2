all:
	gcc main.c -o main -pthread -Wall

run:
	./main 1 3