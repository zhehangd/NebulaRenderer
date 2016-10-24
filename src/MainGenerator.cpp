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

bool generatror_init(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Initialize the volume generator."<<std::endl;
  if(argv.size()<4){
    std::cerr<<"Expected width, height, ks, kv."<<std::endl;
    return false;
  }
  float ks; Console::string_cast(argv[2],ks);
  float kv; Console::string_cast(argv[3],kv);
  unsigned int width;  Console::string_cast(argv[0],width);
  unsigned int height; Console::string_cast(argv[1],height);
  if(width==0 || height==0 || ks<0 || kv<0){
    std::cerr<<"Expected nonzero dimensions.."<<std::endl;
    return false;
  }
  generator.init(width,height,ks,kv);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool generatror_preset_a(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Construct the volume with Preset-Test-A."<<std::endl;
  generator.presetTestA();
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool generatror_preview(Console &console,std::vector<std::string> &argv)
{
  std::cout<<"Save a preview of the material."<<std::endl;
  if(argv.size()<2)
    return Console::meesage_error("Expected filename and slice number.");
  std::string filename = argv[0];
  unsigned int nslice; Console::string_cast(argv[1],nslice);
  if(nslice==0)
    return Console::meesage_error("Expected nonzero slice number.");
  generator.nebula.preview(filename.c_str(),nslice);
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool generatror_save(Console &console,std::vector<std::string> &argv)
{
  
  std::cout<<"Save the material volume."<<std::endl;
  if(argv.size()==0)
    return Console::meesage_error("Expected filename.");
  std::string  filename = argv[0];
  if(!generator.nebula.write(filename.c_str()))
    return Console::meesage_error("Cannot write the volume.");
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

bool generatror_release(Console &console,std::vector<std::string> &argv)
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
  
  console.addCommand("generator_init",         generatror_init);
  console.addCommand("generator_preset_test_a",generatror_preset_a);
  console.addCommand("generator_preview",      generatror_preview);
  console.addCommand("generator_save",         generatror_save);
  console.addCommand("generator_release",      generatror_release); 

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

