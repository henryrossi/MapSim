CXX = g++
CXXFLAGS = -g -Wall -Iinclude -Llib --std=c++17
CC = gcc
CFLAGS = -g -Wall -Werror -Iinclude -Llib
LIBS = -lglfw3 -lgdi32
OBJS = main.o glad.o shader.o stb_image.o

VPATH = src:src/shaders

game : $(OBJS)
	$(CXX) $(CXXFLAGS) -o game $(OBJS) $(LIBS)

main.o : shader.h
glad.o :
stb_image.o :
shader.o : shader.h

.PHONY : clean
clean :
	rm game $(OBJS)