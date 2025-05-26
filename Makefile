
all: simpsocket server client


	
simpsocket:
	gcc -c src/lib/simpsocket.c -Iinclude -o src/lib/simpsocket.o -Wall
	
server:
	gcc src/server.c src/lib/simpsocket.o -Iinclude -o src/server -Wall

client:
	gcc src/client.c src/lib/simpsocket.o -Iinclude -o src/client -Wall

clean:
	$(RM) src/lib/simpsocket.o
	$(RM) src/server
	$(RM) src/client
