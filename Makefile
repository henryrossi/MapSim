CXX = g++
CXXFLAGS = -g -Wall -Werror -Iinclude -Llib
CC = gcc
CFLAGS = -g -Wall -Werror -Iinclude -Llib
LIBS = -lglfw3 -lgdi32
OBJS = main.o glad.o

VPATH = src

game : $(OBJS)
	$(CXX) $(CXXFLAGS) -o game $(OBJS) $(LIBS)

main.o :
glad.o :

.PHONY : clean
clean :
	rm game $(OBJS)