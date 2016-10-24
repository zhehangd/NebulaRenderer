#include "Image.hpp"
#include "Perlin3d.hpp"
#include "VBF.hpp"
#include "Utility.hpp"
#include "Generator.hpp"
#include "Console.hpp"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>

using namespace std;

// Nebula Generator.
Generator generator;

bool cmd_generatror_init(Console &console)
{
  float ks;
  float kv;
  unsigned int width;
  unsigned int height;
  std::cout<<"Initialize the volume generator."<<std::endl;
  bool status = true;
  status &= console.getVariable("volume_width",width);
  status &= console.getVariable("volume_height",height);
  status &= console.getVariable("volume_ks",ks);
  status &= console.getVariable("volume_kv",kv);
  std::cout<<"volume_width  = "<< width <<std::endl;
  std::cout<<"volume_height = "<< height<<std::endl;
  std::cout<<"volume_ks     = "<< ks    <<std::endl;
  std::cout<<"volume_kv     = "<< kv    <<std::endl;
  if(status==false)
    return false;
  generator.init(width,height,ks,kv);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_generatror_preset_a(Console &console)
{
  std::cout<<"Construct the volume with Preset-Test-A."<<std::endl;
  generator.presetTestA();
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_generatror_preview(Console &console)
{
  std::cout<<"Create a preview file."<<std::endl;
  
  std::string  filename;
  unsigned int nslice;
  bool status = true;
  status &= console.getVariable("generator_preview_nslice",nslice);
  status &= console.getVariable("generator_preview_name",filename);
  if(status==false)
    return false;
  std::cout<<"generator_preview_nslice = "<< nslice   <<std::endl;
  std::cout<<"generator_preview_name   = "<< filename <<std::endl;
  generator.nebula.preview(filename.c_str(),nslice);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_generatror_save(Console &console)
{
  std::cout<<"Save the volume."<<std::endl;
  std::string  filename;
  bool status = true;
  status &= console.getVariable("generator_material_name",filename);
  if(status==false)
    return false;
  std::cout<<"generator_material_name = "<< filename <<std::endl;
  generator.nebula.write(filename.c_str());
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool cmd_generatror_release(Console &console)
{
  std::cout<<"Release the volume from the generator."<<std::endl;
  generator.nebula.release();
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}


int main(int argc,const char **argv)
{
  Console console;
  console.ignore_unknown = true;
  
  // ==================== Common ====================
  console.addVariable("volume_width", "64");
  console.addVariable("volume_height","64");
  console.addVariable("volume_ks","100");
  console.addVariable("volume_kv","1");

  // ==================== Volume Generator ====================
  
  
  
  
  console.addVariable("generator_seed",          "20");
  console.addVariable("generator_preview_name",  "preview-m.ppm");
  console.addVariable("generator_preview_nslice","6");
  console.addVariable("generator_material_name", "material.vbf");
  console.addCommand("cmd_generator_init",         cmd_generatror_init);
  console.addCommand("cmd_generator_preset_test_a",cmd_generatror_preset_a);
  console.addCommand("cmd_generator_preview",      cmd_generatror_preview);
  console.addCommand("cmd_generator_save",         cmd_generatror_save);
  console.addCommand("cmd_generator_release",      cmd_generatror_release); 
  
  
  std::cout<<"----------------------------------"<<std::endl;
  
  
  if(argc==1)
  {
    std::cout<<"Please pass script files as arguments."<<std::endl;
    return -1;
  }
  
  for(int i=1;i<argc;i++)
    console.runfile(argv[i]);
  
  return 0;
  
  
  /*
  // Create the volume.
  VBF nebula;
  nebula.set(size,size,2);
  
  // Initialize Perlin3D modules.
  const int depth = 9;
  Perlin3D perlin[2][depth] = {};
  for(int i=0;i<2;i++)
    for(int j=0;j<depth;j++)
      perlin[i][j].setseed(j+i*depth+20);

  // For each voxel.
  for(int i=0;i<nebula.getNumel();i++)
  {
    // Compute the coordinates.
    float xyz[3]; uint16_t uvw[3];
    nebula.getcoord(i,uvw,xyz);
    const float x = xyz[0];
    const float y = xyz[1];
    const float z = xyz[2];
    
    // Compute the noise components.
    float noise[3] = {};
    for(int k=0;k<2;k++){
      for(int d=0;d<depth;d++)
        noise[k] += perlin[k][d].perlin(x+0.25,y+0.25,z+0.25,0.8f/(1<<d))/(1<<d);
    }
    
    float value[2];
    // Compute the material density from the noise.
    for(int k=0;k<2;k++){
      value[k] = std::pow(noise[k],4) * 3;
      value[k] = std::fmin(value[k],1.0f);
    }
    // Set the values.
    nebula.setvalue(uvw[0],uvw[1],uvw[2],value);
    
    // Report.
    if(i%100==0)
      printf("-------- %4.1f%% -------- \r",100.0f*i/nebula.getNumel());
  }*/

  
}

