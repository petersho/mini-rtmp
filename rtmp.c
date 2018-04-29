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
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "rtmp.h"

int host_lookup(RTMP *r, const char *host)
{
	struct addrinfo hints, *res;
	int err;
	void *ptr = NULL;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	err = getaddrinfo (host, NULL, &hints, &res);
	if (err != 0) {
		perror ("getaddrinfo");
		return -1;
	}

	inet_ntop (res->ai_family, res->ai_addr->sa_data, r->addr_str, 100);

	switch (res->ai_family) {
		case AF_INET:
			ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
			break;
	}
	inet_ntop (res->ai_family, ptr, r->addr_str, sizeof(r->addr_str));

	return 0;
}


RTMP *rtmp_alloc()
{
	return malloc(sizeof(RTMP));
}

int rtmp_init(RTMP *r)
{
	memset(r, 0, sizeof(RTMP));
	r->socket = -1;
#if 0
	  r->m_inChunkSize = RTMP_DEFAULT_CHUNKSIZE;
	  r->m_outChunkSize = RTMP_DEFAULT_CHUNKSIZE;
	  r->m_nBufferMS = 30000;
	  r->m_nClientBW = 2500000;
	  r->m_nClientBW2 = 2;
	  r->m_nServerBW = 2500000;
	  r->m_fAudioCodecs = 3191.0;
	  r->m_fVideoCodecs = 252.0;
	  r->Link.timeout = 30;
	  r->Link.swfAge = 30;
#endif
	return 0;
}

int rtmp_send_packet(RTMP *r)
{

	return 0;
}

int rtmp_receive_packet(RTMP *r)
{

	return 0;
}

int rtmp_connect_0(RTMP *r, struct sockaddr_in * server)
{
	int on = 1;

	if (connect(r->socket , (struct sockaddr *)server , sizeof(struct sockaddr_in)) < 0) {
		perror("connect failed. Error");
		return -1;
	}

	setsockopt(r->socket, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));

	return 0;
}

static int gen_rand_data(char *p, int len)
{
	int i = 0;

	if ((p == NULL) || (len <= 0))
		return -1;

	printf("gen rand data size = %d\n", len);
	for (i = 0 ; i < len; i++) {
		*p = (rand() %256);
		p++;
	}

	return 0;
}
int rtmp_handshake(RTMP *r)
{
	struct sockaddr_in server;
	char *receive;
	RTMP_HANDSHAKE_0 *c1;
	RTMP_HANDSHAKE_1 *s2;
	int nbytes = 0;
	int recv_bytes = 0, total_bytes = 3073;
	unsigned char data[97] =
		   {            0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x14, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
			0x07, 0x63, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x03, 0x00, 0x03, 0x61, 0x70, 0x70, 0x02, 0x00, 0x04, 0x6c, 0x69, 0x76, 0x65, 0x00, 0x04,
			0x74, 0x79, 0x70, 0x65, 0x02, 0x00, 0x0a, 0x6e, 0x6f, 0x6e, 0x70, 0x72, 0x69, 0x76, 0x61, 0x74,
			0x65, 0x00, 0x05, 0x74, 0x63, 0x55, 0x72, 0x6c, 0x02, 0x00, 0x15, 0x72, 0x74, 0x6d, 0x70, 0x3a,
			0x2f, 0x2f, 0x6f, 0x73, 0x73, 0x72, 0x73, 0x2e, 0x6e, 0x65, 0x74, 0x2f, 0x6c, 0x69, 0x76, 0x65,
			0x00, 0x00, 0x09
	};

	//Create socket
	r->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (r->socket == -1) {
		printf("Could not create socket");
	}
	printf("Socket created\n");

	server.sin_addr.s_addr = inet_addr(r->addr_str);
	server.sin_family = AF_INET;
	server.sin_port = htons(RTMP_DEFAULT_PORT);

	rtmp_connect_0(r, &server);		//Connect to server

	c1 = malloc(sizeof(RTMP_HANDSHAKE_0));
	if (c1 == NULL)
		return -1;

	s2 = malloc(sizeof(RTMP_HANDSHAKE_1));
	if (s2 == NULL)
		return -1;

	printf("Connected\n");

	c1->ver = 0x03;
	gen_rand_data(c1->data, sizeof(c1->data));

	//Send c0+c1
	nbytes = send(r->socket , c1 ,sizeof(RTMP_HANDSHAKE_0) , 0);
	if (nbytes < 0) {
    	printf("Send failed\n");
    	return 1;
	}
	printf("send c0+c1 nbytes = %d\n", nbytes);

	// Receive s0+s1+s2
	while (1) {
		nbytes = 0;
		//Receive a reply from the server
		nbytes = recv(r->socket, (char *)s2 + recv_bytes, (sizeof(RTMP_HANDSHAKE_1) - nbytes), 0);
		if (nbytes < 0) {
			printf("recv failed\n");
		}
		printf("recv nbytes = %d\n", nbytes);
		recv_bytes += nbytes;
		if (recv_bytes == sizeof(RTMP_HANDSHAKE_1))
			break;
	}

	nbytes = send(r->socket , s2->data1, sizeof(s2->data1) , 0);
	if (nbytes < 0) {
		puts("Send failed\n");
		return 1;
	}
	printf("send c2 nbytes = %d\n", nbytes);

	usleep(100 * 1000);		// sleep 100 ms
	free(c1);
	free(s2);

	//close(r->socket);

	return 0;
}

int test_connect(RTMP *r)
{
	int nbytes = 0;
	unsigned char data[97] =
		   {            0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x14, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
			0x07, 0x63, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x03, 0x00, 0x03, 0x61, 0x70, 0x70, 0x02, 0x00, 0x04, 0x6c, 0x69, 0x76, 0x65, 0x00, 0x04,
			0x74, 0x79, 0x70, 0x65, 0x02, 0x00, 0x0a, 0x6e, 0x6f, 0x6e, 0x70, 0x72, 0x69, 0x76, 0x61, 0x74,
			0x65, 0x00, 0x05, 0x74, 0x63, 0x55, 0x72, 0x6c, 0x02, 0x00, 0x15, 0x72, 0x74, 0x6d, 0x70, 0x3a,
			0x2f, 0x2f, 0x6f, 0x73, 0x73, 0x72, 0x73, 0x2e, 0x6e, 0x65, 0x74, 0x2f, 0x6c, 0x69, 0x76, 0x65,
			0x00, 0x00, 0x09
	};

	nbytes = send(r->socket ,data, 97, 0);
	if (nbytes < 0) {
		puts("Send failed\n");
		return 1;
	}

	printf("send nbytes = %d\n", nbytes);

	return 0;
}

// Application
RTMP *p_rtmp;

int main (int argc, char *argv[])
{
	//rtmp://ossrs.net/live/demo
	char *hostname = "ossrs.net";

	p_rtmp = rtmp_alloc();
	rtmp_init(p_rtmp);

    host_lookup(p_rtmp, hostname);

	printf ("IPv4 address: %s\n", p_rtmp->addr_str);

	rtmp_handshake(p_rtmp);

	//test
	test_connect(p_rtmp);

    return 0;

}
