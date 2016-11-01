#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "VBF.hpp"
#include "Math.hpp"
#include "Perlin3d.hpp"
#include "Image.hpp"
// Nebula Generator.
class Generator
{
public:
  
  // Create the initial volume
  void init(int width,int height,float Ks=100.0f,float Kv=1.0f);
  
  void presetUniform(float m1,float m2);
  
  void presetTestA(void);
  void presetTestB(int seed,float m,float dev,float rat);
  void presetTestC(int seed,float m,float k,float p);
  void presetNebula(void);
  
  void blast(Vector3 src);
  
  VBF nebula;
  
  
  
};

#endif