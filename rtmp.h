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

#define RTMP_DEFAULT_PORT		1935

typedef struct AVal {
	char *av_val;
	int av_len;
}AVal;

typedef struct RTMP_LNK {
	AVal hostname;
}RTMP_LNK;

typedef struct RTMP {
	int socket;
	char addr_str[64];
}RTMP;

typedef struct RTMP_HANDSHAKE_0 {
	char ver;			//c0
	char data[1536];	//c1
}RTMP_HANDSHAKE_0;

typedef struct RTMP_HANDSHAKE_1 {
	char ver;			//s0
	char data1[1536];	//s1
	char data2[1536];	//s2 or c2
}RTMP_HANDSHAKE_1;

