
all: build/test

build/test.o: test.cc
	gcc -g -c -o $@ $<

build/test: build/test.o
	g++ -g -o $@ $<
