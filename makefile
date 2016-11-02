# File:  makefile
# Name:  Zhehang Ding
# Email: dingzhehang1993@gmail.com
# Data:  Oct. 21, 2016


CXX      = g++
DEBUG    = -g
CXXFLAGS = -std=c++11 -O3 -Iinclude
VPATH    = include lib src test
# LXXFLAGS =  $(DEBUG)


.PHONY: clean

ALL   : ngen nren
test  : draw-star triangle

ngen  : Image.o Math.o VBF.o Perlin3d.o Utility.o Generator.o Console.o MainGenerator.o
	$(CXX) -o $@  $^

nren  : Image.o Camera.o Primitive.o Math.o VBF.o Perlin3d.o Renderer.o Utility.o Console.o MainRenderer.o
	$(CXX) -o $@  $^
	

MainRenderer.o :   Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Perlin3d.hpp Console.hpp Renderer.hpp
MainGenerator.o : Image.hpp Math.hpp VBF.hpp Perlin3d.hpp Console.hpp
Renderer.o:  Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Renderer.hpp 
Generator.o: Math.hpp VBF.hpp Image.hpp Perlin3d.hpp

draw-star: Image.o draw-star.o
	$(CXX) -o $@  $^
triangle: Image.o Camera.o Primitive.o Math.o triangle.o
	$(CXX) -o $@  $^

draw-star.o: Image.hpp
triangle.o:  Image.hpp Camera.hpp Primitive.hpp Math.hpp

clean:
	@echo clean
	rm *.o *~ ngen nren draw-star triangle
