TARGET = main
LIBS = -lm
CC = g++
CFLAGS = -g -ggdb -Wall -O3 -std=c++11 -fopenmp
LFLAGS = -g -ggdb -Wall -O3 -fopenmp -lpthread

.PHONY: default all clean

default: $(TARGET)
all: default
#
# asm:
# 	$(CC) -Wa,-adhln  -std=c++11 -g -march=native -masm=intel sort.cpp util.cpp > sort_asm.s

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# .PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
