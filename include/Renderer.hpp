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

  
  // Open, access the material volume
  bool setMaterialVolume(const char *filename){return b_material.read(filename);}
  VBF& getMaterialVolume(void){return b_material;}

  // Open, access the lighting volume
  bool setLightingVolume(const char *filename){return b_lighting.read(filename);}
  VBF& getLightingVolume(void){return b_lighting;}
  
  // Lighting attributes
  void setExtinctionEm2Uv(float k){Ke[0]=k;}
  void setExtinctionEm2Vs(float k){Ke[1]=k;}
  void setAlbedoEm2Uv(float k){Ke[2]=k;}
  void setAlbedoEm2Vs(float k){Ke[3]=k;}
  void setAmbientEm(float k){Ke[4]=k;}
  void setExtinctionRf2Uv(float k){Kr[0]=k;}
  void setExtinctionRf2Vs(float k){Kr[1]=k;}
  void setAlbedoRf2Uv(float k){Kr[2]=k;}
  void setAlbedoRf2Vs(float k){Kr[3]=k;}
  void setAmbientRf(float k){Kr[4]=k;}
  
  float getExtinctionEm2Uv(void){return Ke[0];}
  float getExtinctionEm2Vs(void){return Ke[1];}
  float getAlbedoEm2Uv(void){return Ke[2];}
  float getAlbedoEm2Vs(void){return Ke[3];}
  float getAmbientEm(void){return Ke[4];}
  float getExtinctionRf2Uv(void){return Kr[0];}
  float getExtinctionRf2Vs(void){return Kr[1];}
  float getAlbedoRf2Uv(void){return Kr[2];}
  float getAlbedoRf2Vs(void){return Kr[3];}
  float getAmbientRf(void){return Kr[4];}

  // Pre-compute the lighting field from the material volume.
  void computeLightingVolume(Vector3 src,float Ru,float Rv,float step);

  // Draw a line.
  void drawLine(Vector3 srt,Vector3 end,Vector3 color);
  // Draw a wireframe cube.
  void drawCube(float radius,int tick,Vector3 color);
  // Draw
  void drawOrigin(float radius);
  
  
  

  //
  void drawVolume(float step);

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
