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


bool cmd_renderer_open_material(Console &console);
bool cmd_renderer_open_lighting(Console &console);
bool cmd_renderer_save_material(Console &console);
bool cmd_renderer_save_lighting(Console &console);
bool cmd_renderer_preview_material(Console &console);
bool cmd_renderer_preview_lighting(Console &console);
bool cmd_renderer_create(Console &console);
bool cmd_renderer_compute_lighting(Console &console);
bool cmd_renderer_camera_extrinsic_rectan(Console &console);
bool cmd_renderer_camera_extrinsic_sphere(Console &console);
bool cmd_renderer_camera_intrinsic(Console &console);
bool cmd_renderer_draw_cube(Console &console);
bool cmd_renderer_draw_axes(Console &console);
bool cmd_renderer_draw_volume(Console &console);
bool cmd_renderer_save_canvas(Console &console);


int main(int argc,const char **argv)
{

  Console console;
  console.ignore_unknown = true;

  // ==================== Common ====================
  console.addVariable("volume_width", "64");
  console.addVariable("volume_height","64");
  console.addVariable("volume_ks","100");
  console.addVariable("volume_kv","1");
  
  console.addVariable("canvas_width",  "160");
  console.addVariable("canvas_height", "120");
  console.addVariable("canvas_name",   "output");
  
  // ==================== Lighting Model ====================
  console.addVariable("nebula_extinction_emission"  ,"0.004,0.004");
  console.addVariable("nebula_extinction_reflection","0.004,0.004");
  console.addVariable("nebula_albedo_emission"      ,"0.9,0");
  console.addVariable("nebula_albedo_reflection"    ,"0,0.9");
  console.addVariable("nebula_ambient_emission"     ,"0");
  console.addVariable("nebula_ambient_reflection"   ,"0");
  
  console.addVariable("light_position","0,0,0");
  console.addVariable("light_radiance","1,1");
  
  console.addVariable("camera_extrinsic_from",     "200,200,200");
  console.addVariable("camera_extrinsic_to",       "0,0,0");
  console.addVariable("camera_extrinsic_up",       "0,1,0");
  console.addVariable("camera_extrinsic_azimuth",  "40");
  console.addVariable("camera_extrinsic_altitude", "20");
  console.addVariable("camera_extrinsic_radius",   "400");
  console.addVariable("camera_intrinsic_focus",    "2.2");
  console.addVariable("camera_intrinsic_zmax",     "600");

  console.addVariable("renderer_preview_lighting_name"  ,"preview-l.ppm");
  console.addVariable("renderer_preview_lighting_nslice","6");
  console.addVariable("renderer_preview_material_name"  ,"preview-m.ppm");
  console.addVariable("renderer_preview_material_nslice","6");
  console.addVariable("renderer_material_name"          ,"material.vbf");
  console.addVariable("renderer_lighting_name"          ,"lighting.vbf");
  console.addVariable("renderer_lighting_step"          ,"1");
  console.addVariable("renderer_drawing_step"           ,"1");
  
  console.addCommand("cmd_renderer_create",   cmd_renderer_create);
  console.addCommand("cmd_renderer_open_material",   cmd_renderer_open_material);
  console.addCommand("cmd_renderer_open_lighting",   cmd_renderer_open_lighting);
  console.addCommand("cmd_renderer_save_material",   cmd_renderer_save_material);
  console.addCommand("cmd_renderer_save_lighting",   cmd_renderer_save_lighting);
  console.addCommand("cmd_renderer_preview_material",cmd_renderer_preview_material);
  console.addCommand("cmd_renderer_preview_lighting",cmd_renderer_preview_lighting);
  
  
  console.addCommand("cmd_renderer_camera_extrinsic_rectan", cmd_renderer_camera_extrinsic_rectan);
  console.addCommand("cmd_renderer_camera_extrinsic_sphere", cmd_renderer_camera_extrinsic_sphere);
  console.addCommand("cmd_renderer_camera_intrinsic",   cmd_renderer_camera_intrinsic);
  console.addCommand("cmd_renderer_compute_lighting",   cmd_renderer_compute_lighting);
  console.addCommand("cmd_renderer_draw_cube",     cmd_renderer_draw_cube);
  console.addCommand("cmd_renderer_draw_axes",   cmd_renderer_draw_axes);
  console.addCommand("cmd_renderer_draw_volume",  cmd_renderer_draw_volume);
  console.addCommand("cmd_renderer_save_canvas",  cmd_renderer_save_canvas);
    
  
  
  std::cout<<"----------------------------------"<<std::endl;
  
  
  if(argc==1)
  {
    std::cout<<"Please pass script files as arguments."<<std::endl;
    return -1;
  }
  
  for(int i=1;i<argc;i++)
    console.runfile(argv[i]);
  
  return 0;
  
  float view = -85;
  const char *material = "material.vbf";
  const char *lighting = "lighting.vbf";
  const char *filename = "output.ppm";

  
  // Setup the renderer.
  Renderer render;
  render.setCanvas(640/4,480/4);
  
  // Load volumes.
  render.setMaterialVolume(material);
  render.setLightingVolume(lighting);

  // Setup the camera.
  Camera &camera = render.getCamera();
  camera.setupExt(view,5,400);
  camera.setupInt(2.2,600);
  
  // Draw the cube.
  render.drawCube(100,1,Vector3(1,1,1)*0.2);
  render.drawOrigin(150);
  
  // Draw the volume.
  Timer timer; timer.start();
  render.drawVolume();
  std::cout << "Rendering Time: " << timer.duration() << "s."<<std::endl;;

  for (unsigned int r = 0; r < render.height; r++){
    for (unsigned int c = 0; c < render.width; c++){
      float *pixel = (float*)render.canvas.ptr(r,c);
      for(int k=0;k<3;k++){
        float v = pixel[k];
        v = v * 5;
        v = (exp(3*v)-1)/(exp(3*v)+1);
        pixel[k] = std::fmin(v,1);
      }
    }
  }
  
  imwrite(render.canvas,filename);
  
  return 0;
}

bool cmd_renderer_create(Console &console)
{
  std::cout<<"Create the canvas."<<std::endl;
  unsigned int width;
  unsigned int height;
  bool status = true;
  status &= console.getVariable("canvas_width", width);
  status &= console.getVariable("canvas_height",height);
  if(status==false)
    return false;
  render.setCanvas(width,height);
  return true;
}

bool cmd_renderer_open_material(Console &console)
{
  std::cout<<"Read the material volume."<<std::endl;
  std::string  filename;
  bool status = true;
  status &= console.getVariable("renderer_material_name",filename);
  if(status==false)
    return false;
  std::cout<<"renderer_material_name = "<< filename <<std::endl;
  //
  status &= render.setMaterialVolume(filename.c_str());
  //
  if(status==false)
    return false;
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_open_lighting(Console &console)
{
  std::cout<<"Read the lighting volume."<<std::endl;
  std::string  filename;
  bool status = true;
  status &= console.getVariable("renderer_lighting_name",filename);
  if(status==false)
    return false;
  std::cout<<"renderer_lighting_name = "<< filename <<std::endl;
  //
  status &= render.setLightingVolume(filename.c_str());
  //
  if(status==false)
    return false;
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_save_material(Console &console)
{
  std::cout<<"Save the material volume."<<std::endl;
  std::string  filename;
  bool status = true;
  status &= console.getVariable("renderer_material_name",filename);
  if(status==false)
    return false;
  std::cout<<"renderer_material_name = "<< filename <<std::endl;
  //
  render.getMaterialVolume().write(filename.c_str());
  //
  if(status==false)
    return false;
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_save_lighting(Console &console)
{
  std::cout<<"Save the lighting volume."<<std::endl;
  std::string  filename;
  bool status = true;
  status &= console.getVariable("renderer_lighting_name",filename);
  if(status==false)
    return false;
  std::cout<<"renderer_lighting_name = "<< filename <<std::endl;
  //
  render.getLightingVolume().write(filename.c_str());
  //
  if(status==false)
    return false;
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_preview_material(Console &console)
{
  std::cout<<"Create preview of the material."<<std::endl;
  
  std::string  filename;
  unsigned int nslice;
  bool status = true;
  status &= console.getVariable("renderer_preview_material_nslice",nslice);
  status &= console.getVariable("renderer_preview_material_name",filename);
  if(status==false)
    return false;
  std::cout<<"renderer_preview_material_nslice = "<< nslice   <<std::endl;
  std::cout<<"renderer_preview_material_name   = "<< filename <<std::endl;
  //
  render.getMaterialVolume().preview(filename.c_str(),nslice);
  //
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_preview_lighting(Console &console)
{
  std::cout<<"Create preview of the lighting."<<std::endl;
  
  std::string  filename;
  unsigned int nslice;
  bool status = true;
  status &= console.getVariable("renderer_preview_lighting_nslice",nslice);
  status &= console.getVariable("renderer_preview_lighting_name",filename);
  if(status==false)
    return false;
  std::cout<<"renderer_preview_lighting_nslice = "<< nslice   <<std::endl;
  std::cout<<"renderer_preview_lighting_name   = "<< filename <<std::endl;
  //
  render.getLightingVolume().preview(filename.c_str(),nslice);
  //
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_compute_lighting(Console &console)
{
  std::cout<<"Compute the lighting volume from the material."<<std::endl;
  Vector3 light_pos;
  Vector3 light_rad;
  float step;
  float Ke[5];
  float Kr[5];
  bool status = true;
  status &= console.getVariable("light_position", light_pos.ptr(),3);
  status &= console.getVariable("light_radiance", light_rad.ptr(),2);
  status &= console.getVariable("renderer_lighting_step",  step);
  status &= console.getVariable("nebula_extinction_emission"  ,Ke+0,2);
  status &= console.getVariable("nebula_extinction_reflection",Kr+0,2);
  status &= console.getVariable("nebula_albedo_emission"  ,    Ke+2,2);
  status &= console.getVariable("nebula_albedo_reflection",    Kr+2,2);
  status &= console.getVariable("nebula_ambient_emission"  ,   Ke+4,1);
  status &= console.getVariable("nebula_ambient_reflection",   Kr+4,1);
  if(status==false)
    return false;
  for(int i=0;i<5;i++){
    render.Ke[i] = Ke[i];
    render.Kr[i] = Kr[i];
  }
  std::cout<<"light_position = "<< light_pos <<std::endl;
  std::cout<<"light_radiance = "<< light_rad <<std::endl;
  std::cout<<"renderer_lighting_step  = "<< step <<std::endl;
  std::cout<<"Ke = "; for(int i=0;i<5;i++) std::cout<<" "<<Ke[i]; std::cout<<std::endl;
  std::cout<<"Kr = "; for(int i=0;i<5;i++) std::cout<<" "<<Kr[i]; std::cout<<std::endl;
  //
  render.computeLightingVolume(light_pos,light_rad[0],light_rad[1],1.0f,step);
  //
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_camera_extrinsic_rectan(Console &console)
{
  std::cout<<"Set camera extrinsic parameters."<<std::endl;
  Vector3 cameraFrom;
  Vector3 cameraTo;
  Vector3 cameraUp;
  bool status = true;
  status &= console.getVariable("camera_extrinsic_from",cameraFrom.ptr(),3);
  status &= console.getVariable("camera_extrinsic_to",cameraTo.ptr(),3);
  status &= console.getVariable("camera_extrinsic_up",cameraUp.ptr(),3);
  if(status==false)
    return false;
  std::cout<<"camera_extrinsic_from = "<< cameraFrom <<std::endl;
  std::cout<<"camera_extrinsic_to   = "<< cameraTo   <<std::endl;
  std::cout<<"camera_extrinsic_up   = "<< cameraUp   <<std::endl;
  //
  render.getCamera().setupExt(cameraFrom.ptr(),cameraTo.ptr(),cameraUp.ptr());
  //
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_camera_extrinsic_sphere(Console &console)
{
  std::cout<<"Set camera extrinsic parameters."<<std::endl;
  float azimuth;
  float altitude;
  float radius;
  bool status = true;
  status &= console.getVariable("camera_extrinsic_azimuth", azimuth);
  status &= console.getVariable("camera_extrinsic_altitude",altitude);
  status &= console.getVariable("camera_extrinsic_radius",  radius);
  if(status==false)
    return false;
  std::cout<<"camera_extrinsic_azimuth  = "<< azimuth  <<std::endl;
  std::cout<<"camera_extrinsic_altitude = "<< altitude <<std::endl;
  std::cout<<"camera_extrinsic_radius   = "<< radius   <<std::endl;
  //
  render.getCamera().setupExt(azimuth,altitude,radius);
  //
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_camera_intrinsic(Console &console)
{
  std::cout<<"Set camera intrinsic parameters."<<std::endl;
  float focus;
  float zmax;
  bool status = true;
  status &= console.getVariable("camera_intrinsic_focus", focus);
  status &= console.getVariable("camera_intrinsic_zmax",  zmax);
  if(status==false)
    return false;
  std::cout<<"camera_intrinsic_focus  = "<< focus  <<std::endl;
  std::cout<<"camera_intrinsic_zmax   = "<< zmax <<std::endl;
  render.getCamera().setupInt(focus,zmax);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_draw_cube(Console &console)
{
  std::cout<<"Draw a cube."<<std::endl;
  float ks;
  bool status = true;
  status &= console.getVariable("volume_ks",ks);
  if(status==false)
    return false;
  render.drawCube(ks,1,Vector3(1,1,1));
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_draw_axes(Console &console)
{
  std::cout<<"Draw axes."<<std::endl;
  float ks;
  bool status = true;
  status &= console.getVariable("volume_ks",ks);
  if(status==false)
    return false;
  render.drawOrigin(ks*1.5);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}


bool cmd_renderer_draw_volume(Console &console)
{
  std::cout<<"Render the volume."<<std::endl;
  render.drawVolume();
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_renderer_save_canvas(Console &console)
{
  std::cout<<"Save"<<std::endl;
  
  std::string filename;
  bool status = true;
  status &= console.getVariable("canvas_name",filename);
  if(status==false)
    return false;
  std::cout<<"canvas_name = "<< filename <<std::endl;
  
  for (unsigned int r = 0; r < render.height; r++){
    for (unsigned int c = 0; c < render.width; c++){
      float *pixel = (float*)render.canvas.ptr(r,c);
      for(int k=0;k<3;k++){
        float v = pixel[k];
        v = v * 5;
        v = (exp(3*v)-1)/(exp(3*v)+1);
        pixel[k] = std::fmin(v,1);
      }
    }
  }
  imwrite(render.canvas,filename.c_str());
  
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}
