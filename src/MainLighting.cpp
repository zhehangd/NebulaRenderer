#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <cassert>

#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "VBF.hpp"
#include "Utility.hpp"

class Light
{
public:
  Vector3 src;
  float ultra;
  float visib;
};

#include <vector>

//void (*func)(const std::vector<VBF*> &volumeList,const Vector3 &pos,const Vector3 &dir,float step)

Vector3 fromSrcToDst(Vector3 src,Vector3 dst,VBF &block,float step)
{
  // Direction of the marching.
  Vector3 dir = normalize(dst-src);
  // Compute the distance.
  float dist = (dst-src).norm();
  // Compute the number of steps.
  int   nstep = std::ceil(dist / step);
  // Adjust the step length based on the step number.
  step = dist / nstep;
  // Adjust the starting point so we sample at the center of each step.
  src  = src + dir * step/2;
  
  Vector3 attGas  = Vector3(80,20,80)*3;
  Vector3 attDust = Vector3(10,60,80)*3;
  Vector3 attDark = Vector3(80,80,80)*3;
  Vector3 energy(1.0f,1.0f,1.0f);
  for(int i=0;i<nstep;i++)
  {
    Vector3 val;
    Vector3 pos = src + dir * i * step;
    
    if(block.query(pos.ptr(),val.ptr())==false)
      continue;
    
    Vector3 att = attGas*val[0]+attDust*val[1]+attDark[2]*val[2];

    for(int k=0;k<3;k++)
      energy[k] = energy[k] * std::exp(-att[k]*step);
  }
  return energy;
};

int main(int argc,const char **argv)
{
  
  if(argc==1){
    std::cout<<"Nebula Material Generator\n"
        <<"usage: nlight material lighting\n"
        <<" material  input filename.  (default material.vbf)\n"
        <<" lighting  output filename. (default lighting.vbf)\n"
        <<"This program precompute lighting given the material volume."<<std::endl;
    //return 1;
  }
  
  const char *material = "material.vbf";
  const char *lighting = "lighting.vbf";
  
  if(argc>1)
    material = argv[1];
  if(argc>2)
    lighting = argv[2];  
  
  VBF nebula;
  nebula.read(material);
  nebula.preview("preview-input.ppm",8);
  std::uint32_t numel =  nebula.getNumel();
  std::uint16_t width  = nebula.getWidth();
  std::uint16_t height = nebula.getHeight();
  
  VBF lightfield(width,height,3);
  
  Light light;
  light.src   = Vector3(0,0,0);
  light.ultra = 1;
  light.visib = 1;
  
  for(int i=0;i<numel;i++)
  {
    Vector3  xyz;
    uint16_t uvw[3];
    nebula.getcoord(i,uvw,xyz.ptr());
    
    Vector3 src(0,0,0);
    Vector3 dst = xyz;
    Vector3 val = fromSrcToDst(src,dst,nebula,0.02);
    
    //std::cout<<dst<<std::endl;

    for(int k=0;k<3;k++)
      lightfield.setvalue(uvw[0],uvw[1],uvw[2],val.ptr());
    
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/numel);
    
  }
  nebula.release();
  
  lightfield.preview("preview-output.ppm",8);
  lightfield.write(lighting);
  
  return 0;
}
