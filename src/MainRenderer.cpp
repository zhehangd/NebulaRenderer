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
bool renderer_set_spectrum(Console &console,std::vector<std::string> &argv);


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
  
  
  console.addCommand("renderer_set_spectrum",     renderer_set_spectrum);
  
  console.addCommand("renderer_camera_extrinsic", renderer_camera_extrinsic);
  console.addCommand("renderer_camera_intrinsic",   renderer_camera_intrinsic);
  console.addCommand("renderer_compute_lighting",   renderer_compute_lighting);
  console.addCommand("renderer_draw_cube",     renderer_draw_cube);
  console.addCommand("renderer_draw_axes",    renderer_draw_axes);
  console.addCommand("renderer_draw_volume",  renderer_draw_volume);
  console.addCommand("renderer_save_canvas",  renderer_save_canvas);

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
  console.message_status("Creating the canvas.");
  std::uint16_t width;
  std::uint16_t height;
  if(argv.size()<2)
    return console.message_error("Expected width and height of the canvas.");
  Console::string_cast(argv[0],width);
  Console::string_cast(argv[1],height);
  if(width==0 || height==0)
    return console.message_error("Expected nonzero dimensions.");
  render.setCanvas(width,height);
  return true;
}

bool renderer_setup_extinct(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Setting extinction coefficients.");
  if(argv.size()==0 || ((argv[0]!="Emission")&&(argv[0]!="Reflection")))
    return console.message_error("Expected type \"Emission\" or \"Reflection\"");
  if(argv.size()<2)
    return console.message_error("Expected coefficients.");
  float K[5]; Console::string_cast(argv[1],K,5);
  if(argv[0]=="Emission")
    for(int i=0;i<5;i++)
      render.Ke[i] = K[i];
  else
    for(int i=0;i<5;i++)
      render.Kr[i] = K[i];
  return true;
}


bool renderer_open_material(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Reading the material volume.");
  if(argv.size()==0)
    return console.message_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.setMaterialVolume(filename.c_str()))
    return console.message_error("Cannot read the volume.");
  return true;
}

bool renderer_open_lighting(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Read the lighting volume.");
  if(argv.size()==0)
    return console.message_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.setLightingVolume(filename.c_str()))
    return console.message_error("Cannot read the volume.");
  return true;
}

bool renderer_save_material(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Save the material volume.");
  if(argv.size()==0)
    return console.message_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.getMaterialVolume().write(filename.c_str()))
    return console.message_error("Cannot write the volume.");
  return true;
}

bool renderer_save_lighting(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Save the lighting volume.");
  if(argv.size()==0)
    return console.message_error("Expected filename.");
  std::string  filename = argv[0];
  if(!render.getLightingVolume().write(filename.c_str()))
    return console.message_error("Cannot write the volume.");
  return true;
}

bool renderer_preview_material(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Save a preview of the material.");
  if(argv.size()<2)
    return console.message_error("Expected filename and slice number.");
  std::string filename = argv[0];
  unsigned int nslice; Console::string_cast(argv[1],nslice);
  if(nslice==0)
    return console.message_error("Expected nonzero slice number.");
  render.getMaterialVolume().preview(filename.c_str(),nslice);
  return true;
}

bool renderer_preview_lighting(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Create preview of the lighting.");
  if(argv.size()<2)
    return console.message_error("Expected filename and slice number.");
  std::string filename = argv[0];
  unsigned int nslice; Console::string_cast(argv[1],nslice);
  if(nslice==0)
    return console.message_error("Expected nonzero slice number.");
  render.getLightingVolume().preview(filename.c_str(),nslice);
  return true;
}

bool renderer_compute_lighting(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Compute the lighting volume from the material.");
  if(argv.size()<2)
    return console.message_error("Expected step and at least one light.");

  float step;       Console::string_cast(argv[0],step);
  std::vector<Light> lights;
  for(int i=1;i<argv.size();i++)
  {
    float atts[5]; Console::string_cast(argv[i],atts,5);
    lights.push_back(Light(Vector3(atts),atts[3],atts[4]));
  }
  
  render.computeLightingVolume(lights,step);
  return true;
}


bool renderer_camera_extrinsic(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Set camera extrinsic parameters.");
  
  if(argv.size()==0 || ((argv[0]!="Rect")&&(argv[0]!="Polar")))
    return console.message_error("Expected mode \"Rect\" or \"Polar\"");
  
  if(argv[0]=="Rect")
  {
    if(argv.size()<4)
      return console.message_error("Expected LookFrom, LookAt, and LookUp vectors.");
    Vector3 lookfrom; Console::string_cast(argv[1],lookfrom.ptr(),3);
    Vector3 lookat;   Console::string_cast(argv[2],lookat.ptr()  ,3);
    Vector3 lookup;   Console::string_cast(argv[3],lookup.ptr()  ,3);
    render.getCamera().setupExt(lookfrom.ptr(),lookat.ptr(),lookup.ptr());
  }
  else
  {
    if(argv.size()<4)
      return console.message_error("Expected azimuth, altitude, and radius.");
    float azimuth;  Console::string_cast(argv[1],azimuth);
    float altitude; Console::string_cast(argv[2],altitude);
    float radius;   Console::string_cast(argv[3],radius);
    if(radius==0)
      return console.message_error("Expected nonzero radius.");
    render.getCamera().setupExt(azimuth,altitude,radius);
  }
  return true;
}

bool renderer_camera_intrinsic(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Set camera intrinsic parameters.");
  if(argv.size()<2)
    return console.message_error("Expected focus and zmax.");
  float focus; Console::string_cast(argv[0],focus);
  float zmax;  Console::string_cast(argv[1],zmax);
  if(focus<=0 || zmax<=0)
    return console.message_error("focus and zmax should be positive.");
  render.getCamera().setupInt(focus,zmax);
  return true;
}

bool renderer_draw_cube(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Draw a cube.");
  if(argv.size()<1)
    return console.message_error("Expected size.");
  float size; Console::string_cast(argv[0],size);
  if(size<=0)
    return console.message_error("size should be positive.");
  render.drawCube(size,1,Vector3(1,1,1));
  return true;
}

bool renderer_draw_axes(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Draw axes.");
  if(argv.size()<1)
    return console.message_error("Expected size.");
  float size; Console::string_cast(argv[0],size);
  if(size<=0)
    return console.message_error("size should be positive.");
  render.drawOrigin(size);
  return true;
}


bool renderer_draw_volume(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Render the volume.");
  if(argv.size()<1)
    return console.message_error("Expected step.");
  float step;  Console::string_cast(argv[0],step);
  if(step<0)
    return console.message_error("Expected positive steps.");
  // Draw the volume.
  Timer timer; timer.start();
  render.drawVolume(step);
  std::cout << "Rendering Time: " << timer.duration() << "s."<<std::endl;
  return true;
}

bool renderer_save_canvas(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Save the canvas");
  if(argv.size()==0)
    return console.message_error("Expected filename.");
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
  return true;
}

bool renderer_set_spectrum(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Set spectrum");
  if(argv.size()<2)
    return console.message_error("Expected two spectrum vectors");
  
  Vector3 emission;   Console::string_cast(argv[0],emission.ptr(),3);
  Vector3 reflection; Console::string_cast(argv[1],reflection.ptr(),3);
  render.setSpectrumEmission(emission);
  render.setSpectrumReflection(reflection);
  return true;
}