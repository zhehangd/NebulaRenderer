#ifndef RENDER_H_ 
#define RENDER_H_

#include <cstdint>

#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "VBF.hpp"

class Renderer
{
public:
    
  // Specify the dimension of the canvas.
  // Execute this as an initialization.
  void setup(int width,int height);
  
  // Replace the camera configuration by the given one.
  void setCamera(const Camera &cam){camera=cam;}
  // Get the reference of the current camera.
  Camera& getCamera(void){return camera;}

  // Draw a line.
  void drawLine(Vector3 srt,Vector3 end,Vector3 color);
  
  // Draw a wireframe cube.
  void drawCube(float radius,int tick,Vector3 color);
  
  // Load the material volume.
  bool loadVolumeMaterial(const char* filename);

  // Load the lighting volume.
  bool loadVolumeLighting(const char* filename);
  
  void setVolumeScale(float ks);

  //
  void drawVolume(float ke,float ka,float step);

  std::uint16_t width;
  std::uint16_t height;
  
  // Frame data.
  ImFloat canvas;
  // Depth buffer.
  ImFloat depth;
  // Camera.
  Camera camera;
  
  // Material volume.
  VBF b_material;
  // Lighting volume.
  VBF b_lighting;
  
  
private:
  
  bool loadVolume(VBF &volume,const char* filename);
  
};



#endif
