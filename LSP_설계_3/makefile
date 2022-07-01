ssu_sfinder : ssu_sfinder.o ssu_func.o
	gcc -o ssu_sfinder ssu_sfinder.o ssu_func.o -lcrypto -pthread

ssu_sfinder.o : ssu_sfinder.c
		gcc -c -o ssu_sfinder.o ssu_sfinder.c -lcrypto -pthread

ssu_func.o : ssu_func.c
		gcc -c -o ssu_func.o ssu_func.c -lcrypto -pthread


clean :
		rm *.o ssu_sfinder
		rm *.o ssu_func

