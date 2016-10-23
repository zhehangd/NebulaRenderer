#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "VBF.hpp"
#include "Math.hpp"
#include "Perlin3d.hpp"

// Nebula Generator.
class Generator
{
public:
  
  // Create the initial volume
  void init(int width,int height,float m1,float m2);
  
  void presetTestA(void);
  
  //void init(int width,int height,int seed,float m,float dev,float rat);
  
  void blast(Vector3 src);
  
  VBF nebula;
  
  
  
};

#endif