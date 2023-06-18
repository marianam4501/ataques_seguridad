CC = g++
CFLAGS = -Wall -g -std=c++17 -lnsl -lstdc++fs -lcrypto 

server: 
		$(CC) main.cpp Encrypt.cpp Receptor.cpp -o server -lssl -lcrypto -lstdc++fs

client: 
		$(CC) -Wall -g main.cpp -o main 

clean:
		$(RM) Sender server client Channel

main:
		g++ main.cpp Encrypt.cpp Sender.cpp Client.cpp -o Sender -lssl -lcrypto -lstdc++fs

channel:
		g++ main.cpp Channel.cpp Client.cpp -o Channel -lssl -lcrypto -lstdc++fs

