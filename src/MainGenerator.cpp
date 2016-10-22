#include "Image.hpp"
#include "Perlin3d.hpp"
#include "VBF.hpp"
#include "Utility.hpp"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>

using namespace std;

int main(int argc,const char **argv)
{

  if(argc==1){
    cout<<"Nebula Material Generator\n"
        <<"usage: ngen size filename seed\n"
        <<" size      size of the volume to generate. (default 64)\n"
        <<" filename  output filename. (default material.vbf)\n"
        <<" seed      random seed. (defualt 20)\n"
        <<"This program generates a random volume of nebula. Currently\n"
        <<"this program only has basic function -- it simply uses perlin\n"
        <<"noise as the material distribution."<<endl;
    //return 1;
  }
  
  int seed = 20;
  int size = 64;
  const char* filename = "material.vbf";
  
  if(argc>1)
    size = atoi(argv[1]);
  
  if(argc>2)
    filename = argv[2];
  
  if(argc>3)
    seed = atoi(argv[3]);
  
  // Create the volume.
  VBF nebula;
  nebula.set(size,size,2);
  
  // Initialize Perlin3D modules.
  const int depth = 9;
  Perlin3D perlin[2][depth] = {};
  for(int i=0;i<2;i++)
    for(int j=0;j<depth;j++)
      perlin[i][j].setseed(j+i*depth+20);

  // For each voxel.
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
    for(int k=0;k<2;k++){
      for(int d=0;d<depth;d++)
        noise[k] += perlin[k][d].perlin(x+0.25,y+0.25,z+0.25,0.8f/(1<<d))/(1<<d);
    }
    
    float value[2];
    // Compute the material density from the noise.
    for(int k=0;k<2;k++){
      value[k] = std::pow(noise[k],4) * 3;
      value[k] = std::fmin(value[k],1.0f);
    }
    // Set the values.
    nebula.setvalue(uvw[0],uvw[1],uvw[2],value);
    
    // Report.
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/nebula.getNumel());
  }
  printf("\n");
  nebula.preview("m-preview.ppm",6);
  nebula.write(filename);

  return 0;
}

