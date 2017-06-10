aes: aes.o aes_util.o
	gcc -o aes -g aes.o aes_util.o -lcrypto -lm

aes.o: aes.c
	gcc -g -c -Wall aes.c 

aes_utlil.o: aes_util.c
	gcc -g -c -Wall aes_util.c

clean:
	rm -f *.o aes
