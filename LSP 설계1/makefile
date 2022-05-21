ssu_sindex : main.o ssu_sindex.o
	gcc -o ssu_sindex main.o ssu_sindex.o

main.o : main.c
		gcc -c -o main.o main.c

ssu_sindex.o : ssu_sindex.c
		gcc -c -o ssu_sindex.o ssu_sindex.c

clean :
		rm *.o ssu_sindex