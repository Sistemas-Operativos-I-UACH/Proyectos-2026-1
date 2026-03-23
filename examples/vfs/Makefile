dirs:
	mkdir -p bin

build: dirs
	gcc read_file.c -o bin/read_file

test: build
	bin/read_file /etc/passwd

clean:
	rm -rf bin

