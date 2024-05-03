CXX = g++
CXXFLAGS = -g -Wall -Iinclude -Llib -L. --std=c++17
CC = gcc
CFLAGS = -g -Wall -Iinclude -Llib
LIBS = -lglfw3 -lgdi32 -lassimp-5
OBJS = main.o glad.o shader.o stb_image.o mapcamera.o flycamera.o model.o

VPATH = src

game : $(OBJS)
	$(CXX) $(CXXFLAGS) -o game $(OBJS) $(LIBS)

main.o : shader.hpp mapcamera.hpp flycamera.hpp model.hpp
glad.o :
stb_image.o :
shader.o : shader.hpp
model.o : model.hpp
mapcamera.o : mapcamera.hpp
flycamera.o : flycamera.hpp

.PHONY : clean
clean :
	rm game $(OBJS)