GCC = g++
CFLAGS = -Wall -g

all: mync ttt

mync: mync.o
	$(GCC) $(CFLAGS) -o mync mync.o

mync.o: mync.cpp
	$(GCC) $(CFLAGS) -c mync.cpp

ttt: ttt.o
	$(GCC) $(CFLAGS) -o ttt ttt.o

ttt.o: ttt.cpp
	$(GCC) $(CFLAGS) -c ttt.cpp
tar2 : 
	./mync -e "ttt 123456789"
tar3.1 : #Open a server at port 4050 and connect to it from a client using terminal"
	echo "you need to 'make runClient' in a new terminal"
	./mync -e "ttt 123456789" -i TCPS4050
tar3.2 : #Open a server at port 4050 and make a server and a client that will tlak to each other with the same port while using the 4050 server
	echo "you need to 'make runServer' first in a new terminal, than 'make runClient' in a second terminal"
	./mync -e "ttt 123456789" -i TCPS4050 -o TCPClocalhost,4455
tar3.5 : #Open a server and a client for sending messages from the client to the server
	echo "you need to 'make runServer' first in a new terminal, than 'make runClient' in a second terminal"
	./mync -i TCPS4050 -o TCPClocalhost,4455
	
tar4 : #Open a UDP server at port 4455 and connect to it from a client using terminal"
	echo "you need to 'make runClient_tar4' in a new terminal"
	./mync -e "ttt 123456789" -i UDPS4455 -t 10
	
tar4.1: #Open an UDP server at port 4050 and connect to it from a client
	echo "you need to 'make runServer' in a new terminal, than 'make runUDPClient in a second terminal"
	./mync -e "ttt 123456789" -i UDPS4050 -o TCPClocalhost,4455
tar6 : 
runServer:
	nc -l -p 4455
runClient:
	nc localhost 4455
runClient_tar4:
	nc -u localhost 4455
runUDPClient:
	nc -u localhost 4050
runUDPServer:
	nc -u -l -p 4455
runServerStream:
	./mync -i UDSSS/tmp/uds_stream_server
runClientStream :
	./mync -o UDSCS/tmp/uds_stream_server
runClientStream_ttt :
	./mync -o UDSCS/tmp/uds_stream_server -e "ttt 123456789"



clean:
	-pkill -9 mync
	rm -f mync ttt *.o
