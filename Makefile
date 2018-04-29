all:
	gcc -O2 -Wall -o rtmp rtmp.c

debug:
	gcc -g -O0 -Wall -o rtmp rtmp.c

clean:
	rm -f rtmp
