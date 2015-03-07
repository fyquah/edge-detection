OBJS = lodepng/lodepng.o main.o
CXX = g++
FLAGS = -ansi -pedantic -Wall -Wextra -O3
TARGET = ./main

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(FLAGS)

clean:
	rm -rf *.o