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

ngen  : Image.o Math.o VBF.o Perlin3d.o Utility.o Generator.o Console.o MainGenerator.o
	$(CXX) -o $@  $^

nren  : Image.o Camera.o Primitive.o Math.o VBF.o Perlin3d.o Renderer.o Utility.o Console.o MainRenderer.o
	$(CXX) -o $@  $^
	

MainRenderer.o :   Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Perlin3d.hpp Renderer.hpp
MainGenerator.o : Image.hpp Math.hpp VBF.hpp Perlin3d.hpp
Renderer.o:  Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Renderer.hpp 
Utility.o: Math.hpp
Generator.o: Math.hpp VBF.hpp Perlin3d.hpp

mt-vbf: Image.o VBF.o mt-vbf.o
	$(CXX) -o $@  $^
mt-console: Console.o mt-console.o
	$(CXX) -o $@  $^

mt-vbf.o: Image.hpp VBF.hpp
mt-console.o : Console.hpp
clean:
	@echo clean
	rm *.o *~ ngen nren mt-vbf mt-console
