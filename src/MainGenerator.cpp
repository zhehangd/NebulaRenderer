#include "Image.hpp"
#include "Perlin3d.hpp"
#include "VBF.hpp"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>

using namespace std;

int main(void)
{
  VBF nebula;

  // Specify the block parameters.
  int size = 512;
  nebula.set(size,size,3);
  
  // Initialize Perlin3D.
  const int depth = 9;
  Perlin3D perlin[3][depth] = {};
  for(int i=0;i<3;i++)
    for(int j=0;j<depth;j++)
      perlin[i][j].setseed(j+i*depth+20);

    
  float maxValue = 0;
  float minValue = 1;

  for(int i=0;i<nebula.getNumel();i++)
  {
    // Compute the coordinates.
    float xyz[3]; uint16_t uvw[3];
    nebula.getcoord(i,uvw,xyz);
    const float x = xyz[0];
    const float y = xyz[1];
    const float z = xyz[2];
    
    // Compute the noise components.
    float noise[3] = {};
    for(int k=0;k<3;k++){
      for(int d=0;d<depth;d++)
        noise[k] += perlin[k][d].perlin(x+0.25,y+0.25,z+0.25,0.8f/(1<<d))/(1<<d);
    }
    
    float value[3];
    // Compute the values from the noise.
    //for(int k=0;k<3;k++)
    //  value[k] = std::pow(noise[k],4) * 3;
    // Clamp the values.
    for(int k=0;k<3;k++)
      value[k] = std::fmin(noise[k]+0.5f,1.0f);
    // Set the values.
    nebula.setvalue(uvw[0],uvw[1],uvw[2],value);
    
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/nebula.getNumel());
  }
  printf("\n");
  nebula.preview("Preview.ppm",6);
  nebula.write("block.vbf");

  return 0;
}

