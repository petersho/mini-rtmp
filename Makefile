all:
	gcc -g -O2 -Wall -o rtmp rtmp.c

clean:
	rm -f rtmp
