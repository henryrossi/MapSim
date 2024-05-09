CXX = g++
CXXFLAGS = -g -Wall -Iinclude -Llib --std=c++17
CC = gcc
CFLAGS = -g -Wall -Iinclude -Llib
LIBS = -lglfw3 -lgdi32 -lassimp -lzlibstatic
OBJS = glad.o shader.o stb_image.o mapcamera.o flycamera.o model.o mesh.o noise.o

VPATH = src

game : main.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o game main.o $(OBJS) $(LIBS)

main.o : shader.hpp mapcamera.hpp flycamera.hpp model.hpp noise.hpp
glad.o :
stb_image.o :
shader.o : shader.hpp
model.o : model.hpp
mesh.o : model.hpp
mapcamera.o : mapcamera.hpp
flycamera.o : flycamera.hpp
noise.o : noise.hpp

.PHONY : clean
clean :
	rm game $(OBJS)