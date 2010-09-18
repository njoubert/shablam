TARGET = shablam
SOURCES = $(wildcard ./src/*.cpp)
HEADERS = $(wildcard ./src/*.h)

CXX = g++

INCLUDE = -I/opt/local/include
LIB = -lmad
LIBDIRS = -L/opt/local/lib

%.o : %.cpp
	$(CXX) -c -Wall $(INCLUDE) $< -o $@

OBJECTS = $(SOURCES:.cpp=.o)

all: $(HEADERS) $(SOURCES) $(OBJECTS)
	$(CXX) $(LIBDIRS) $(LIB) -o $(TARGET) $(OBJECTS)

clean:
	rm -rf *.o $(TARGET)