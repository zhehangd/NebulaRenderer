#include "Generator.hpp"
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

void Generator::presetTestA(void)
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


void Generator::presetTestB(int seed,float m,float dev,float rat)
{  
  // Initialize the Perlin generator.
  const int depth = 9;
  Perlin3D perlin[2][depth] = {};
  for(int i=0;i<2;i++)
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
    float noise[2] = {0,0};
    for(int j=0;j<2;j++)
      noise[j] = SynPerlin(perlin[j],depth,xyz,60,0);

    float density[2];
    density[0] = dev*noise[0] + m;
    density[1] = dev*noise[1] + m * rat;
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

void Generator::presetTestC(int seed,float m,float k,float p)
{  
  k = k / 2 * 2;
  
  // Initialize the Perlin generator.
  const int depth = 9;
  Perlin3D perlin[2][depth] = {};
  for(int i=0;i<2;i++)
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
    float base  = 60;  // base frequency.
    float sharp = 0.0f; // enhance the high frequencies.
    float noise[2] = {0,0};
    for(int k=0;k<2;k++){
      for(int d=0;d<depth;d++){
        float t = 1.0f / (float)((unsigned int)1<<d);
        float value;
        value  = perlin[k][d].perlin(xyz[0],xyz[1],xyz[2],t*base);
        value *= t*(1+sharp*d);
        noise[k] += value;
      }
    }

    float density[2];
    density[0] = k*std::exp(noise[0]*p) + m;
    density[1] = k*std::exp(noise[1]*p) + m;
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

/*
void Generator::blast(Vector3 src)
{
  for(int i=0;i<nebula.getNumel();i++)
  {
    float value[2];
    // Compute the coordinates.
    float xyz[3]; uint16_t uvw[3];
    nebula.getcoord(i,uvw,xyz);
    nebula.getvalue(uvw[0],uvw[1],uvw[2],value);
    
    Vector3 dst(xyz);
    float dist = (dst-src).norm();
    dist = 15.0f / (dist + 15.0f) * 0.08;

    for(int k=0;k<2;k++){
      value[k] -= dist;
        
      value[k] = std::fmin(value[k],1);
      value[k] = std::fmax(value[k],0);
    }
    
    nebula.setvalue(uvw[0],uvw[1],uvw[2],value);
  }
  
};*/