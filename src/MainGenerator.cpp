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
  console.message_status("Initialize the volume generator.");
  if(argv.size()<4)
    console.message_error("Expected width, height, ks, kv.");
  float ks; Console::string_cast(argv[2],ks);
  float kv; Console::string_cast(argv[3],kv);
  unsigned int width;  Console::string_cast(argv[0],width);
  unsigned int height; Console::string_cast(argv[1],height);
  if(width==0 || height==0 || ks<0 || kv<0)
    return console.message_error("Expected nonzero dimensions..");
  generator.init(width,height,ks,kv);
  
  return true;
}

bool generatror_preset_a(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Construct the volume with Preset-Test-A.");
  generator.presetTestA();
  
  return true;
}

bool generatror_preset_b(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Construct the volume with Preset-Test-B.");
  if(argv.size()<4)
    return console.message_error("Expected seed, mean, noise, and ratio.");
  int seed;   Console::string_cast(argv[0],seed);
  float mean; Console::string_cast(argv[1],mean);
  float dev;  Console::string_cast(argv[2],dev);
  float ratio; Console::string_cast(argv[3],ratio);
  generator.presetTestB(seed,mean,dev,ratio);
  return true;
}

bool generatror_preset_c(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Construct the volume with Preset-Test-B.");
  if(argv.size()<4)
    return console.message_error("Expected seed, mean, k, and p.");
  int seed;   Console::string_cast(argv[0],seed);
  float mean; Console::string_cast(argv[1],mean);
  float k;    Console::string_cast(argv[2],k);
  float p ;   Console::string_cast(argv[3],p);
  generator.presetTestC(seed,mean,k,p);
  return true;
}

bool generatror_preset_nebula(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Construct the volume with Preset-Nebula (Under Construction).");
  generator.presetNebula();
  return true;
}

bool generatror_preview(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Save a preview of the material.");
  if(argv.size()<2)
    return console.message_error("Expected filename and slice number.");
  std::string filename = argv[0];
  unsigned int nslice; Console::string_cast(argv[1],nslice);
  if(nslice==0)
    return console.message_error("Expected nonzero slice number.");
  generator.nebula.preview(filename.c_str(),nslice);
  
  return true;
}

bool generatror_save(Console &console,std::vector<std::string> &argv)
{
  
  console.message_status("Save the material volume.");
  if(argv.size()==0)
    return console.message_error("Expected filename.");
  std::string  filename = argv[0];
  if(!generator.nebula.write(filename.c_str()))
    return console.message_error("Cannot write the volume.");
  
  return true;
}

bool generatror_release(Console &console,std::vector<std::string> &argv)
{
  console.message_status("Release the volume from the generator.");
  generator.nebula.release();
  
  return true;
}


int main(int argc,const char **argv)
{
  Console console;
  console.ignore_unknown = true;
  
  console.addCommand("generator_init",         generatror_init);
  console.addCommand("generator_preset_test_a",generatror_preset_a);
  console.addCommand("generator_preset_test_b",generatror_preset_b);
  console.addCommand("generator_preset_test_c",generatror_preset_c);
  console.addCommand("generatror_preset_nebula",generatror_preset_nebula);
  console.addCommand("generator_preview",      generatror_preview);
  console.addCommand("generator_save",         generatror_save);
  console.addCommand("generator_release",      generatror_release); 

  
  
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

