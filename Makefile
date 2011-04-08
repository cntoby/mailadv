OBJS = sendmail.o getopt.o base64.o main.o
CFLAGS = -Wall -static-libgcc -static -pthread

All: mailadv
mailadv: $(OBJS)
	gcc -o mailadv $(OBJS) $(CFLAGS) 

sendmail.o: sendmail/sendmail.c sendmail/sendmail.h
	gcc -c sendmail/sendmail.c -o sendmail.o $(CFLAGS)

getopt.o: getopt/getopt.c getopt/getopt.h
	gcc -c getopt/getopt.c -o getopt.o $(CFLAGS)

base64.o: base64/base64.c base64/base64.h
	gcc -c base64/base64.c -o base64.o $(CFLAGS)

main.o: main.c main.h
	gcc -c main.c -o main.o $(CFLAGS)

clean:
	rm -f *.o
