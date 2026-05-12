CC=gcc
CFLAGS=-m32 -fno-stack-protector -z execstack -g

all: vuln

vuln: vuln.c
	$(CC) $(CFLAGS) vuln.c -o vuln

clean:
	rm -f vuln
