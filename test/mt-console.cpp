#include "Console.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>

bool cmd_generator_init(Console &console,std::vector<std::string> &argv)
{
  if(argv.size()<2)
    return false;
  std::cout<<"--------- TEST COMMANDS ----------"<<std::endl;
  unsigned int width;
  unsigned int height;
  Console::string_cast(argv[0],width);
  Console::string_cast(argv[1],height);
  std::cout<<"width  = "<<width <<std::endl;
  std::cout<<"height = "<<height<<std::endl;
  std::cout<<"----------------------------------"<<std::endl;
  return true;
}

int main(void)
{
  
  Console console;
  
  console.addVariable("name","");
  console.addVariable("e-mail","");
  console.addVariable("test","");
  
  console.show();
  std::cout<<"---------------------"<<std::endl;
  
  console.eval("name   = Zhehang Ding");
  console.eval("e-mail = zhehangd@usc.edu");
  console.eval("email = mydzh@usc.edu");
  
  std::cout<<"---------------------"<<std::endl;
  
  console.show();
  std::cout<<"---------------------"<<std::endl;
  
  console.addVariable("vector1","12.3, 42.1, 87,412 ,241,");
  console.addVariable("vector2","12.3,21");
  console.addVariable("vector3","1");
  
  float vec[3];
  console.getVariable("vector1",vec,3,',');
  std::cout<<"("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
  console.getVariable("vector2",vec,3,',');
  std::cout<<"("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
  console.getVariable("vector3",vec,3,',');
  std::cout<<"("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
  
  console.clear();
  
  std::cout<<"---------------------"<<std::endl;
  console.addVariable("width","64");
  console.addVariable("height","64");
  console.addCommand("generator_init",cmd_generator_init);
  
  std::cout<<console.eval("generator_init 42 abc")<<std::endl;
  
  console.show();
  
  
  std::string st = "TEST";
  std::cout<<(st=="TEST")<<std::endl;
  std::cout<<(st==st)<<std::endl;
  
  return 0;
};

