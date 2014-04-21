 udp:
	gcc udp-server.c -o udpserver -lsqlite3
	gcc udp-client.c -o udpclient
	
tcp:
	gcc tcp-server.c -o tcpserver -lsqlite3
	gcc tcp-client.c -o tcpclient
clean:
	rm -f udpserver udpclient tcpserver tcpclient

