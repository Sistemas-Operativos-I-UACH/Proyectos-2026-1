dirs:
	mkdir -p bin

build: dirs
	gcc proc.c -o bin/proc

test: build
	bin/proc

clean:
	rm -rf bin

