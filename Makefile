
TARGET := shablam

INCLUDE := -I/opt/local/include
LIB := -lmad
LIBDIRS := -L/opt/local/lib

all: main.o
	g++ $(LIBDIRS) $(LIB) -o $(TARGET) main.o

main.o: main.cpp
	g++ $(INCLUDE) -o main.o -c main.cpp

clean:
	rm -rf *.o $(TARGET)