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
  
  const char *fileMaterial = "material.vbf";
  const char *fileLighting = "lighting.vbf";
  
  if(argc>1)
    fileMaterial = argv[1];
  if(argc>2)
    fileLighting = argv[2];  
  
  Renderer render;
  render.setMaterialVolume(fileMaterial);
  
  std::cout<<"width  = "<<render.b_material.getWidth()<<std::endl;
  std::cout<<"height = "<<render.b_material.getHeight()<<std::endl;
  std::cout<<"Ks = "<<render.b_material.getKs()<<std::endl;
  std::cout<<"Kv = "<<render.b_material.getKv()<<std::endl;
  
  render.computeLightingVolume(Vector3(0,0,100),1.0f,1.0f,1.0f,2.0f);
  
  render.getLightingVolume().write(fileLighting);
  render.getLightingVolume().preview("preview-l.ppm",6);
  
  
  return 0;
}
