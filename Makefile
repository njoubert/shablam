
TARGET := shazam

INCLUDE := -I/opt/local/include
LIB := -lmad
LIBDIRS := -L/opt/local/lib

main.o: main.cpp
	g++ $(INCLUDE) -o main.o -c main.cpp

all: main.o
	g++ $(LIBDIRS) $(LIB) -o $(TARGET) main.o

clean:
	rm -rf *.o $(TARGET)