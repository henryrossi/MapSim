CXX = g++
CXXFLAGS = -g -Wall -Iinclude -Llib --std=c++17
CC = gcc
CFLAGS = -g -Wall -Werror -Iinclude -Llib
LIBS = -lglfw3 -lgdi32
OBJS = main.o glad.o shader.o stb_image.o mapcamera.o

VPATH = src:src/shaders

game : $(OBJS)
	$(CXX) $(CXXFLAGS) -o game $(OBJS) $(LIBS)

main.o : shader.hpp mapcamera.hpp
glad.o :
stb_image.o :
shader.o : shader.hpp
mapcamera.o : mapcamera.hpp

.PHONY : clean
clean :
	rm game $(OBJS)