
all: build/test build-release/test

build/test.o: test.cc unordered_bits.h unordered_set.h unordered_map.h
	gcc -g -c -o $@ $<

build/test: build/test.o
	g++ -g -o $@ $<

build-release/test.o: test.cc unordered_bits.h unordered_set.h unordered_map.h
	gcc -O3 -c -o $@ $<

build-release/test: build-release/test.o
	g++ -o $@ $<

