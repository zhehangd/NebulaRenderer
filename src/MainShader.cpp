#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>

#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "Primitive.hpp"
#include "Perlin3d.hpp"
#include "VBF.hpp"
#include "Shader.hpp"


std::ostream& operator<<(std::ostream& ss,const Vector3 &v)
{
  ss<<std::setprecision(2)<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<")";
  return ss;
}

#include <chrono>

class Timer
{
public:
  void start(void){srt=std::chrono::steady_clock::now();}
  float duration(void){return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - srt).count();}
  std::chrono::steady_clock::time_point srt;
};


int main(int argc,const char **argv)
{
  float view = 192;
  const char *filename = "output.ppm";

  if(argc>1)
    view = atof(argv[1]);
  if(argc>2)
    filename = argv[2];
  
  // Setup the renderer.
  Renderer render;
  render.setup(640,480);
  
  // Load volumes.
  render.loadVolumeMaterial("../dat/Nebula-0M.vbf");
  render.loadVolumeLighting("../dat/Nebula-0L4.vbf");
  render.setVolumeScale(100);
 
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
