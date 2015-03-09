OBJS = file/file.o lodepng/lodepng.o jpeg/jpeg.o png/png.o main.o 
CXX = g++
FLAGS = -ansi -pedantic -Wall -Wextra -O3 -ljpeg -std=c++11 -lmagic
TARGET = ./main

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(FLAGS)

clean:
	rm -rf *.o