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
#include "Renderer.hpp"

class Light
{
public:
  Vector3 src;
  float ultra;
  float visib;
};

#include <vector>

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

  Renderer render;
  render.setMaterialVolume(material);
  render.setVolumeScale(100);
  
  render.Ke[0] = 2.0; // Extinction coefficient for UV radiance.
  render.Ke[1] = 0.6; // Extinction coefficient for visible radiance.
  render.Ke[2] = 0.8; // Albedo for UV radiance.
  render.Ke[3] = 0.1; // Albedo for visible radiance.
  render.Kr[0] = 2.0; // Extinction coefficient for UV radiance.
  render.Kr[1] = 0.6; // Extinction coefficient for visible radiance.
  render.Kr[2] = 0.0; // Albedo for UV radiance.
  render.Kr[3] = 0.6; // Albedo for visible radiance.
  
  render.computeLightingVolume();
  render.getLightingVolume().write(lighting);
  
  return 0;
}
