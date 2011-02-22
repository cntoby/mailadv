OBJS = sendmail.o getopt.o base64.o main.o

All: mailadv
mailadv: $(OBJS)
	gcc -o mailadv $(OBJS)

sendmail.o: sendmail/sendmail.c sendmail/sendmail.h
	gcc -c sendmail/sendmail.c -o sendmail.o

getopt.o: getopt/getopt.c getopt/getopt.h
	gcc -c getopt/getopt.c -o getopt.o

base64.o: base64/base64.c base64/base64.h
	gcc -c base64/base64.c -o base64.o

main.o: main.c main.h
	gcc -c main.c -o main.o

clean:
	rm -f *.o
