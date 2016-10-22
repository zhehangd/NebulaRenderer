# File:  makefile
# Name:  Zhehang Ding
# Email: dingzhehang1993@gmail.com
# Data:  Oct. 21, 2016


CXX      = g++
DEBUG    = -g
CXXFLAGS = -std=c++11 -O3 -Iinclude
VPATH    = include lib src
# LXXFLAGS =  $(DEBUG)


.PHONY: clean

ALL   : shad gen light

shad  : Image.o Camera.o Primitive.o Math.o VBF.o Perlin3d.o Shader.o MainShader.o
	$(CXX) -o $@  $^

gen  : Image.o Math.o VBF.o Perlin3d.o MainGenerator.o
	$(CXX) -o $@  $^
	
light: Image.o Camera.o Math.o VBF.o Primitive.o Shader.o MainLighting.o
	$(CXX) -o $@  $^

MainShader.o :   Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Perlin3d.hpp Shader.hpp
MainGenerator.o : Image.hpp Math.hpp VBF.hpp Perlin3d.hpp
MainLighting.o :  Image.hpp Camera.hpp Math.hpp VBF.hpp Shader.hpp
Shader.o: Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Shader.hpp 

clean:
	@echo clean
	rm *.o *~ shad gen light
