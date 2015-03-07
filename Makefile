OBJS = lodepng/lodepng.o jpeg/jpeg.o main.o
CXX = g++
FLAGS = -ansi -pedantic -Wall -Wextra -O3 -ljpeg
TARGET = ./main

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(FLAGS)

clean:
	rm -rf *.o