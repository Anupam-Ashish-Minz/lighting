CC=g++
LIBS=-lglfw -lGLEW -lGL -lX11 -lGLU -lOpenGL

build:
	$(CC) -g -o main main.cpp $(LIBS)
