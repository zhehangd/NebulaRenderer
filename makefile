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

ALL   : ngen nren nlight

ngen  : Image.o Math.o VBF.o Perlin3d.o Utility.o MainGenerator.o
	$(CXX) -o $@  $^

nren  : Image.o Camera.o Primitive.o Math.o VBF.o Perlin3d.o Shader.o Utility.o MainShader.o
	$(CXX) -o $@  $^
	
nlight: Image.o Camera.o Math.o VBF.o Primitive.o Shader.o Utility.o MainLighting.o
	$(CXX) -o $@  $^

MainShader.o :   Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Perlin3d.hpp Shader.hpp
MainGenerator.o : Image.hpp Math.hpp VBF.hpp Perlin3d.hpp
MainLighting.o :  Image.hpp Camera.hpp Math.hpp VBF.hpp Shader.hpp
Shader.o:  Image.hpp Camera.hpp Primitive.hpp Math.hpp VBF.hpp Shader.hpp 
Utility.o: Math.hpp


mt-vbf: Image.o VBF.o mt-vbf.o
	$(CXX) -o $@  $^

mt-vbf.o: Image.hpp VBF.hpp

clean:
	@echo clean
	rm *.o *~ ngen nren nlight mt-vbf
