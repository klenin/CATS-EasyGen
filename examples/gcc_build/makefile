PARSER_LIB = ../../parser
CPP_LIB = ../../cpp

all: gen

gen: gen.o easygen.o parselib.a
	g++ gen.o easygen.o parselib.a -o gen

gen.o:
	g++ -c gen.cpp -I$(CPP_LIB) -I$(PARSER_LIB)

easygen.o:
	g++ -c $(CPP_LIB)/easygen.cpp -I$(CPP_LIB) -I$(PARSER_LIB)

parselib.a: parser.o
	ar rvs parselib.a parser.o

parser.o:
	gcc -c $(PARSER_LIB)/parser.c -I$(PARSER_LIB)

clean:
	rm -f parselib.a parser.o easygen.o gen.o gen.exe

