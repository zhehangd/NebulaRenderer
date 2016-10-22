# File:  makefile
# Name:  Zhehang Ding
# Email: dingzhehang1993@gmail.com
# Data:  Feb. 08, 2016


CPP       = g++
DEBUG    = -g
CXXFLAGS =  -std=c++11 -O3
# LXXFLAGS =  $(DEBUG)

.PHONY: ALL shad gen light clean

ALL   : shad gen light

shad  : Image.o Camera.o Primitive.o Math.o VBF.o Perlin3d.o Shader.o MainShader.o
	$(CPP) -o shad  Image.o Camera.o Primitive.o Math.o VBF.o Perlin3d.o Shader.o MainShader.o

gen  : Image.o Math.o VBF.o Perlin3d.o MainGenerator.o
	$(CPP) -o gen  Image.o Math.o VBF.o Perlin3d.o MainGenerator.o
	
light: Image.o Camera.o Math.o VBF.o Shader.o MainLighting.o
	$(CPP) -o light  Image.o Camera.o Math.o VBF.o Shader.o Primitive.o MainLighting.o

MainShader.o :   Image.h Camera.h Primitive.h Math.h VBF.h Perlin3d.h Shader.h MainShader.cpp
MainGenerator.o : Image.h Math.h VBF.h Perlin3d.h MainGenerator.cpp
MainLighting.o :  Image.h Camera.h Math.h VBF.h Shader.h MainLighting.cpp
Shader.o: Image.h Camera.h Primitive.h Math.h VBF.h Shader.h Shader.cpp

clean:
	rm *.o *~ shad gen light
