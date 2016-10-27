#include "Camera.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include "Primitive.hpp"
#include "Perlin3d.hpp"
#include "VBF.hpp"
#include "Renderer.hpp"
#include "Utility.hpp"
#include "Console.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>

Renderer render;


bool renderer_open_material(Console &console,std::vector<std::string> &argv);
bool renderer_open_lighting(Console &console,std::vector<std::string> &argv);
bool renderer_save_material(Console &console,std::vector<std::string> &argv);
bool renderer_save_lighting(Console &console,std::vector<std::string> &argv);
bool renderer_preview_material(Console &console,std::vector<std::string> &argv);
bool renderer_preview_lighting(Console &console,std::vector<std::string> &argv);
bool renderer_setup_canvas(Console &console,std::vector<std::string> &argv);
bool renderer_setup_extinct(Console &console,std::vector<std::string> &argv);
bool renderer_compute_lighting(Console &console,std::vector<std::string> &argv);
bool renderer_camera_extrinsic(Console &console,std::vector<std::string> &argv);
bool renderer_camera_intrinsic(Console &console,std::vector<std::string> &argv);
bool renderer_draw_cube(Console &console,std::vector<std::string> &argv);
bool renderer_draw_axes(Console &console,std::vector<std::string> &argv);
bool renderer_draw_volume(Console &console,std::vector<std::string> &argv);
bool renderer_save_canvas(Console &console,std::vector<std::string> &argv);


int main(int argc,const char **argv)
{

  Console console;
  console.ignore_unknown = true;

  console.addCommand("renderer_setup_canvas",    renderer_setup_canvas);
  console.addCommand("renderer_setup_extinct",   renderer_setup_extinct);
  console.addCommand("renderer_open_material",   renderer_open_material);
  console.addCommand("renderer_open_lighting",   renderer_open_lighting);
  console.addCommand("renderer_save_material",   renderer_save_material);
  console.addCommand("renderer_save_lighting",   renderer_save_lighting);
  console.addCommand("renderer_preview_material",renderer_preview_material);
  console.addCommand("renderer_preview_lighting",renderer_preview_lighting);
  
  console.addCommand("renderer_camera_extrinsic", renderer_camera_extrinsic);
  console.addCommand("renderer_camera_intrinsic",   renderer_camera_intrinsic);
  console.addCommand("renderer_compute_lighting",   renderer_compute_lighting);
  console.addCommand("renderer_draw_cube",     renderer_draw_cube);
  console.addCommand("renderer_draw_axes",    renderer_draw_axes);
  console.addCommand("renderer_draw_volume",  renderer_draw_volume);
  console.addCommand("renderer_save_canvas",  renderer_save_canvas);

  std::cout<<"----------------------------------"<<std::endl;

  if(argc==1)
  {
    std::cout<<"Please pass script files as arguments."<<std::endl;
    return -1;
  }
  
  for(int i=1;i<argc;i++)
  {
    if(std::string(argv[i])=="-e" && (i!=(argc-1)))
      console.eval(argv[i+1]);
    else
      console.runfile(argv[i]);
  }
  
  return 0;
}

bool renderer_setup_canvas(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Create the canvas."<<std::endl;
  std::uint16_t width;
  std::uint16_t height;
  if(argv.size()<2){
    std::cerr<<"Expected width and height of the canvas."<<std::endl;
    return false;
  }
  Console::string_cast(argv[0],width);
  Console::string_cast(argv[1],height);
  if(width==0 || height==0){
    std::cerr<<"Expected nonzero dimensions.."<<std::endl;
    return false;
  }
  render.setCanvas(width,height);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_setup_extinct(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Set extinction coefficients."<<std::endl;
  
  if(argv.size()==0 || ((argv[0]!="Emission")&&(argv[0]!="Reflection")))
    return Console::meesage_error("Expected type \"Emission\" or \"Reflection\"");
  if(argv.size()<2)
    return Console::meesage_error("Expected coefficients.");
  float K[5]; Console::string_cast(argv[1],K,5);
  if(argv[0]=="Emission")
    for(int i=0;i<5;i++)
      render.Ke[i] = K[i];
  else
    for(int i=0;i<5;i++)
      render.Kr[i] = K[i];
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}


bool renderer_open_material(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Read the material volume."<<std::endl;
  if(argv.size()==0)
    return Console::meesage_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.setMaterialVolume(filename.c_str()))
    return Console::meesage_error("Cannot read the volume.");
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_open_lighting(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Read the lighting volume."<<std::endl;
  if(argv.size()==0)
    return Console::meesage_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.setLightingVolume(filename.c_str()))
    return Console::meesage_error("Cannot read the volume.");
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_save_material(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Save the material volume."<<std::endl;
  if(argv.size()==0)
    return Console::meesage_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.getMaterialVolume().write(filename.c_str()))
    return Console::meesage_error("Cannot write the volume.");
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_save_lighting(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Save the lighting volume."<<std::endl;
  if(argv.size()==0)
    return Console::meesage_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.getLightingVolume().write(filename.c_str()))
    return Console::meesage_error("Cannot write the volume.");
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_preview_material(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Save a preview of the material."<<std::endl;
  if(argv.size()<2)
    return Console::meesage_error("Expected filename and slice number.");
  std::string filename = argv[0];
  unsigned int nslice; Console::string_cast(argv[1],nslice);
  if(nslice==0)
    return Console::meesage_error("Expected nonzero slice number.");
  render.getMaterialVolume().preview(filename.c_str(),nslice);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_preview_lighting(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Create preview of the lighting."<<std::endl;
  if(argv.size()<2)
    return Console::meesage_error("Expected filename and slice number.");
  std::string filename = argv[0];
  unsigned int nslice; Console::string_cast(argv[1],nslice);
  if(nslice==0)
    return Console::meesage_error("Expected nonzero slice number.");
  render.getLightingVolume().preview(filename.c_str(),nslice);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_compute_lighting(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Compute the lighting volume from the material."<<std::endl;
  
  if(argv.size()<3)
    return Console::meesage_error("Expected position, radiance, and step.");
  
  Vector3 position; Console::string_cast(argv[0],position.ptr(),3);
  Vector3 radiance; Console::string_cast(argv[1],radiance.ptr(),2);
  float step;       Console::string_cast(argv[2],step);

  render.computeLightingVolume(position,radiance[0],radiance[1],step);
  //
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}


bool renderer_camera_extrinsic(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Set camera extrinsic parameters."<<std::endl;
  
  if(argv.size()==0 || ((argv[0]!="Rect")&&(argv[0]!="Polar")))
    return Console::meesage_error("Expected mode \"Rect\" or \"Polar\"");
  
  if(argv[0]=="Rect")
  {
    if(argv.size()<4)
      return Console::meesage_error("Expected LookFrom, LookAt, and LookUp vectors.");
    Vector3 lookfrom; Console::string_cast(argv[1],lookfrom.ptr(),3);
    Vector3 lookat;   Console::string_cast(argv[2],lookat.ptr()  ,3);
    Vector3 lookup;   Console::string_cast(argv[3],lookup.ptr()  ,3);
    render.getCamera().setupExt(lookfrom.ptr(),lookat.ptr(),lookup.ptr());
  }
  else
  {
    if(argv.size()<4)
      return Console::meesage_error("Expected azimuth, altitude, and radius.");
    float azimuth;  Console::string_cast(argv[1],azimuth);
    float altitude; Console::string_cast(argv[2],altitude);
    float radius;   Console::string_cast(argv[3],radius);
    if(radius==0)
      return Console::meesage_error("Expected nonzero radius.");
    render.getCamera().setupExt(azimuth,altitude,radius);
  }
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_camera_intrinsic(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Set camera intrinsic parameters."<<std::endl;
  if(argv.size()<2)
    return Console::meesage_error("Expected focus and zmax.");
  float focus; Console::string_cast(argv[0],focus);
  float zmax;  Console::string_cast(argv[1],zmax);
  if(focus<=0 || zmax<=0)
    return Console::meesage_error("focus and zmax should be positive.");
  render.getCamera().setupInt(focus,zmax);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_draw_cube(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Draw a cube."<<std::endl;
  if(argv.size()<1)
    return Console::meesage_error("Expected size.");
  float size; Console::string_cast(argv[0],size);
  if(size<=0)
    return Console::meesage_error("size should be positive.");
  render.drawCube(size,1,Vector3(1,1,1));
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_draw_axes(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Draw axes."<<std::endl;
  if(argv.size()<1)
    return Console::meesage_error("Expected size.");
  float size; Console::string_cast(argv[0],size);
  if(size<=0)
    return Console::meesage_error("size should be positive.");
  render.drawOrigin(size);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}


bool renderer_draw_volume(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Render the volume."<<std::endl;
  if(argv.size()<1)
    return Console::meesage_error("Expected step.");
  float step;  Console::string_cast(argv[0],step);
  if(step<0)
    return Console::meesage_error("Expected positive steps.");
  // Draw the volume.
  Timer timer; timer.start();
  render.drawVolume(step);
  std::cout << "Rendering Time: " << timer.duration() << "s."<<std::endl;;
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool renderer_save_canvas(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Save the canvas"<<std::endl;
  if(argv.size()==0){
    std::cerr<<"Expected filename."<<std::endl;
    return false;
  }
  std::string filename = argv[0];
  // Tone mapping is temporarily put here.
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
  imwrite(render.canvas,filename.c_str());
  
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}
