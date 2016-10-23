#include "Console.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>


int main(void)
{
  
  Console console;
  
  console.addVariable("name","");
  console.addVariable("e-mail","");
  console.addVariable("test","");
  
  console.show();
  std::cout<<"---------------------"<<std::endl;
  
  console.eval("name = Zhehang Ding");
  console.eval(" ");
  console.eval(" # this is a comment.");
  console.eval("e-mail = zhehangd@usc.edu  ");
  console.eval("test = Nessun Dorma. ");
  console.eval("  bad =  ");
  console.eval("  bad2=");
  console.eval(" ");
  console.eval("shade");
  console.eval(" name  ");
  console.eval(" emial  ");
  console.eval(" e-mail  ");
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
  return 0;
};

