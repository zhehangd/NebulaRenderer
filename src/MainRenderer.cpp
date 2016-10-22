#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "Primitive.hpp"
#include "Perlin3d.hpp"
#include "VBF.hpp"
#include "Renderer.hpp"
#include "Utility.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>

int main(int argc,const char **argv)
{
  if(argc==1){
    std::cout<<"Nebula Renderer\n"
        <<"usage: nren material lighting view outputfile\n"
        <<" material    material volume file. (default material)\n"
        <<" lighting    lighting volume file. (default lighting)\n"
        <<" view        horizontal observation angle. (default 192)\n"
        <<" outputfile  output filename. (default output.ppm)\n"
        <<"This program render the nebula given the material and\n"
        <<"lighting volume."<<std::endl;
    //return 1;
  }
  
  float view = 45;
  const char *material = "material.vbf";
  const char *lighting = "lighting.vbf";
  const char *filename = "output.ppm";

  if(argc>1)
    material = argv[1];
  if(argc>2)
    lighting = argv[2];
  if(argc>3)
    view = atof(argv[3]);
  if(argc>4)
    filename = argv[4];
  
  // Setup the renderer.
  Renderer render;
  render.setCanvas(640/4,480/4);
  
  // Load volumes.
  render.setMaterialVolume(material);
  render.setLightingVolume(lighting);
  render.setVolumeScale(100);

  render.Ke[0] = 2.0; // Extinction coefficient for UV radiance.
  render.Ke[1] = 0.6; // Extinction coefficient for visible radiance.
  render.Ke[2] = 0.8; // Albedo for UV radiance.
  render.Ke[3] = 0.1; // Albedo for visible radiance.
  render.Kr[0] = 2.0; // Extinction coefficient for UV radiance.
  render.Kr[1] = 0.6; // Extinction coefficient for visible radiance.
  render.Kr[2] = 0.0; // Albedo for UV radiance.
  render.Kr[3] = 0.6; // Albedo for visible radiance.

  // Setup the camera.
  Camera &camera = render.getCamera();
  camera.setupExt(view,30,400);
  camera.setupInt(2.2,600);
  
  // Draw the cube.  
  render.drawCube(100,1,Vector3(1,1,1)*0.2);
  
  // Draw the volume.
  Timer timer; timer.start();
  render.drawVolume(0.8,0.2,1);
  std::cout << "Rendering Time: " << timer.duration() << "s."<<std::endl;;

  for (unsigned int r = 0; r < render.height; r++){
    for (unsigned int c = 0; c < render.width; c++){
      float *pixel = (float*)render.canvas.ptr(r,c);
      for(int k=0;k<3;k++){
        float v = pixel[k];
        v = v * 2;
        v = (exp(3*v)-1)/(exp(3*v)+1);
        pixel[k] = std::fmin(v,1);
      }
    }
  }
  
  imwrite(render.canvas,filename);
  
  return 0;
}
