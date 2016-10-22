#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <cassert>

#include "Camera.h"
#include "Image.h"
#include "Math.h"
#include "VBF.h"


class Timer
{
public:
  void start(void){srt=std::chrono::steady_clock::now();}
  float duration(void){return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - srt).count();}
  std::chrono::steady_clock::time_point srt;
};

class Light
{
public:
  Vector3 src;
  float ultra;
  float visib;
};

std::ostream& operator<<(std::ostream& ss,const Vector3 &v)
{
  ss<<std::setprecision(2)<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<")";
  return ss;
}

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

  //std::cout<<"SRC: "<<src<<std::endl;
  //std::cout<<"DST: "<<dst<<std::endl;
  //std::cout<<"DIR: "<<dir<<std::endl;
  //std::cout<<"--------------------"<<std::endl;
  
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

int main(void)
{
  VBF nebula;
  nebula.read("../data/Nebula-0M.vbf");
  nebula.preview("preview-input.ppm",4);
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

    for(int k=0;k<3;k++)
      lightfield.setvalue(uvw[0],uvw[1],uvw[2],val.ptr());
    
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/numel);
    
  }
  nebula.release();
  
  lightfield.preview("preview-output.ppm",8);
  lightfield.write("output.vbf");
  
  return 0;
}
