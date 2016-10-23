#ifndef RENDERER_H_ 
#define RENDERER_H_

#include <cstdint>

#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "VBF.hpp"

class Renderer
{
public:
  
  Renderer(void);
  
  // Specify the dimension of the canvas.
  // Execute this as an initialization.
  void setCanvas(int width,int height);
  
  // Replace the camera configuration by the given one.
  void setCamera(const Camera &cam){camera=cam;}
  // Get the reference of the current camera.
  Camera& getCamera(void){return camera;}

  
  bool setMaterialVolume(const char *filename){return b_material.read(filename);}
  VBF& getMaterialVolume(void){return b_material;}

  bool setLightingVolume(const char *filename){return b_lighting.read(filename);}
  VBF& getLightingVolume(void){return b_lighting;}
  // Pre-compute the lighting field from the material volume.
  void computeLightingVolume(Vector3 src,float Ru,float Rv,float att,float step);
  
  // Set Ks for all volumes.
  void setVolumeScale(float ks);

  // Draw a line.
  void drawLine(Vector3 srt,Vector3 end,Vector3 color);
  // Draw a wireframe cube.
  void drawCube(float radius,int tick,Vector3 color);
  // Draw
  void drawOrigin(float radius);
  

  //
  void drawVolume(void);

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
  
  
  // Parameters that model emission nabulae and
  // reflection nebulae.
  // [0]: extinction coefficient for ultraviolet radiance.
  // [1]: extinction coefficient for visible radiance.
  // [2]: albedo for ultraviolet radiance.
  // [3]: albedo for visible radiance.
  // [4]: ambient radiance.
  float Ke[5];
  float Kr[5];
  
private:
  
};



#endif
