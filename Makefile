
all: build/test

build/test.o: test.cc unordered_bits.h unordered_set.h unordered_map.h
	gcc -g -c -o $@ $<

build/test: build/test.o
	g++ -g -o $@ $<
