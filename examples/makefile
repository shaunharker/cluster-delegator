# makefile for distrib project                                             
CC := mpicxx
CXX := mpicxx
LDFLAGS := -lboost_serialization
CXXFLAGS := -O3 -std=c++11 -I../include/ 

all: example1 example2

example1: example1.o
example2: example2.o

.PHONY: clean
clean:
	rm -f *.o
	rm -f example1
	rm -f example2
	rm -f *~
