/*
Copyright (C) 2018-2019 Peter Kung(peterkoug@gmail.com) All Rights Reserved

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char addrstr[64];

int host_lookup(const char *host)
{
	struct addrinfo hints, *res;
	int err;
	void *ptr;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	err = getaddrinfo (host, NULL, &hints, &res);
	if (err != 0) {
		perror ("getaddrinfo");
		return -1;
	}

	inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

	switch (res->ai_family) {
		case AF_INET:
			ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
			break;
	}
	inet_ntop (res->ai_family, ptr, addrstr, 64);

	return 0;
}

int handshake()
{
	int sock;
	struct sockaddr_in server;
	//char message[1537], receive[3073];
	char *message, *receive;

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	printf("Socket created\n");
     
	server.sin_addr.s_addr = inet_addr(addrstr);
	server.sin_family = AF_INET;
	server.sin_port = htons(1935);
 
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

	message = malloc(1537);
	if (message == NULL) {
		return -1;
	}
	receive = malloc(3073);
	if (receive == NULL) {
		return -1;
	}

	printf("Connected\n");

	message[0] = 0x03;
	message[1] = 0xde;
	message[1536] = 0xaa;
	//Send some data
	if( send(sock , message ,1537 , 0) < 0) {
    	printf("Send failed\n");
    	return 1;
	}
	//Receive a reply from the server
	if( recv(sock , receive , 3073 , 0) < 0) {
		printf("recv failed\n");
	}
	if(send(sock , receive + 1, 1536 , 0) < 0) {
		puts("Send failed\n");
		return 1;
	}

	//sleep(1);
	//closesocket(sock);

	return 0;
}

int main (int argc, char *argv[])
{
	//rtmp://ossrs.net/live/demo
	char *hostname = "ossrs.net";


    memset(addrstr, 0, sizeof(addrstr));
    host_lookup(hostname);

	printf ("IPv4 address: %s\n", addrstr);

	handshake();

    return 0;

}
