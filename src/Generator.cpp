#include "Generator.hpp"
#include "Image.hpp"
#include "Math.hpp"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath> 


// Synthesize Pink noise from Perlin noise.
float SynPerlin(const Perlin3D *perlin,int depth,float *xyz,float base,float sharp)
{
  float value = 0;
  for(int d=0;d<depth;d++){
    float k = 1.0f / (float)((unsigned int)1<<d);
    float v;
    v  = perlin[d].perlin(xyz[0],xyz[1],xyz[2],k*base);
    v *= k*(1+sharp*d);
    value += v;
  }
  return value;
}


void Generator::init(int width,int height,float Ks,float Kv)
{
  nebula.set(width,height,2);
  nebula.setKs(Ks);
  nebula.setKv(Kv);
}

void Generator::presetUniform(float m1,float m2)
{
  float value[2] = {m1,m2};
  for(int i=0;i<nebula.getNumel();i++){
    nebula.setvalue(i,value);
  }
}

void Generator::presetTest(void)
{
  float preset[4][2] = {{0,0.1},{0,1},{1,0},{0.1,0}};
  std::uint16_t width  = nebula.getWidth();
  std::uint16_t height = nebula.getHeight();
  for(int i=0;i<nebula.getNumel();i++){
    std::uint16_t uvw[3]; nebula.getcoord(i,uvw);
    bool f1 = uvw[0] > (width  / 2);
    bool f2 = uvw[1] > (height / 2);
    if (f1 && f2)
      nebula.setvalue(i,preset[0]);
    else
      if ((!f1) && f2)
        nebula.setvalue(i,preset[1]);
      else
        if (f1 && (!f2))
          nebula.setvalue(i,preset[2]);
        else
          nebula.setvalue(i,preset[3]);
  }
}

// Generate nebula.
// Currently the parameters are hard coded.
void Generator::presetNebula(void)
{
  // Four componetns.
  // 1. Large clusters of emission nebula.
  // 2. Large clusters of reflection nebula.
  // 3. Sparse reflection mist.
  // 4. Small dense dark nebula.
  
  int seed = 20;
  float m = 0;
  float dev = 1;
  float rat = 1;
  
  // Initialize the Perlin generator.
  const int pch = 4; // Number of channels of Perlin noise.
  const int depth   = 9;
  Perlin3D perlin[pch][depth] = {};
  for(int i=0;i<pch;i++)
    for(int j=0;j<depth;j++)
      perlin[i][j].setseed(j+i*depth+seed);

  //
  // For each voxel.
  for(int i=0;i<nebula.getNumel();i++)
  {
    // Compute the coordinates.
    float xyz[3]; uint16_t uvw[3];
    nebula.getcoord(i,uvw,xyz);    
    // Compute the noise components.
    float noise[pch] = {0,0};
    for(int j=0;j<pch;j++)
      noise[j] = SynPerlin(perlin[j],depth,xyz,60,0.3);
    
    float density[2] = {};
    density[0] += std::exp(8*(noise[0]-0.3)-0.2); density[0] = std::fmin(density[0],1.0f); density[0] = std::fmax(density[0],0.0f);
    density[1] += std::exp(8*(noise[1]-0.3)-0.2); density[1] = std::fmin(density[1],1.0f); density[1] = std::fmax(density[1],0.0f);
    density[1] += noise[3]*0.1 - 0.03;
    
    // Synthesize the density values.
    for(int k=0;k<2;k++){
      float value = density[k];
      value = std::fmax(value,0.0f);
      value = std::fmin(value,1.0f);
      density[k] = value;
    }
    
    // Set the values.
    nebula.setvalue(uvw[0],uvw[1],uvw[2],density);
    
    // Report.
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/nebula.getNumel());
  }  
  
}

float quatIteration(Quaternion q,Quaternion c,int thres)
{
    // If the coordinates exceed a threshold.
    // We directly return false to save time.
    if(q.norm1()>10)
        return 0;
    for(int i=0;i<thres;i++)
    {
        q = q*q + c;
        if(q.norm1()>10.0f)
            return std::pow((float)i/thres,2);
    }
    return 1.0f;
}

void Generator::presetJulia(void)
{
  
  const int depth   = 9;
  Perlin3D perlin[depth] = {};
  for(int j=0;j<depth;j++)
      perlin[j].setseed(j+10);
  
  // For each voxel.
  for(int i=0;i<nebula.getNumel();i++)
  {
    // Compute the coordinates.
    float xyz[3]; uint16_t uvw[3];
    nebula.getcoord(i,uvw,xyz);    
    
    
    float density[2] = {};
    
    density[0] = 0.0;
    density[1] = 0.0;
    
    Quaternion q(xyz[0],xyz[1],xyz[2],0);
    Quaternion c(-0.2,0.8,0.0,0.0);
    q *= 1.40f/nebula.getKs();
    
    //float aad = 1.4f*2.0f/nebula.getWidth();
    // Antialiasing.
   // Quaternion dq[6] = {
   //   {aad,0,0,0},{-aad,0,0,0},{0,aad,0,0},
    //  {0,-aad,0,0},{0,0,aad,0},{0,0,-aad,0}
    //};
    //
    
    float qvalue = quatIteration(q,c,20);// * 0.6f;
    //if(qvalue!=0)
    //  for(int i=0;i<6;i++)
    //    qvalue += quatIteration(q+dq[i],c,20) * (0.4f / 6.0f);
    
    
    if(qvalue==0)
      continue;
    
    if(qvalue>=0.9)
      density[0] = qvalue*1.0f;
    else
      density[1] = qvalue * (1+SynPerlin(perlin,depth,xyz,60,0.3)*2);
    
    // Set the values.
    nebula.setvalue(uvw[0],uvw[1],uvw[2],density);
    
    // Report.
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/nebula.getNumel());
  }  
  
}