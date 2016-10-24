#include "Console.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>

Console::Console(void)
{
  ignore_unknown = false;
}

bool Console::addVariable(std::string name,std::string value)
{
  auto iter = dict_v.find(name);
  if ( iter!= dict_v.end() ){
    meesage_has_existed(name);
    return false;
  }
  dict_v.insert(std::make_pair(name,value));
  return true;
}

bool Console::addCommand(std::string name,bool (*f)(Console&))
{
  auto iter  = dict_v.find(name);
  if ( iter != dict_v.end() ){
    meesage_has_existed(name);
    return false;
  }
  dict_c.insert(std::make_pair(name,f));
  return true;
}

void Console::string_trim_blank(std::string &line)
{
  const char *blank = " \t\n\r\f\v";
  line.erase(0, line.find_first_not_of(blank));
  line.erase(line.find_last_not_of(blank)+1);
}

void Console::string_trim_comment(std::string &line)
{
  size_t pos = line.find_first_of("#");
  if (pos != std::string::npos)
    line = line.erase(pos);
}

// Evaluate one line of string (which should contain only one directive).
bool Console::eval(std::string line)
{
  // Trim the string.
  string_trim_blank(  line);
  string_trim_comment(line);
  
  if (line.empty())
    return true;
  
  
  std::string name,value;
  
  // Variable
  size_t eq = line.find_first_of("=");
  if (eq != std::string::npos)
  {
    
    name  = line.substr(0,eq); string_trim_blank(name);
    value = line.substr(eq+1); string_trim_blank(value);
    auto it = dict_v.find(name);
    if ( it != dict_v.end() ){
      it->second = value;
      meesage_variable(name,value);
      return true;
    }
  }
  else
  {
    // Command
    name = line;
    auto it  = dict_c.find(line);
    if(  it != dict_c.end() )
      return it->second(*this);
    else
    {
      // Variable Query
      auto it = dict_v.find(line);
      if (it != dict_v.end()){
        meesage_variable(it->first,it->second);
        return true;
      }
    }
  }
  
  meesage_not_found(name);
  return ignore_unknown;
}

// Evaluate lines in a file.
void Console::runfile(const char *filename)
{
  std::ifstream ss(filename);
  while(1)
  {
      std::string line;
      std::getline(ss,line);
      if(ss.good()==false)
          break;
      if(eval(line)==false){
        std::cout<<"Error, execution halts."<<std::endl;
        break;
      }
  }
}

void Console::show(void)
{
  for(auto it : dict_v)
    meesage_variable(it.first,it.second);
  for(auto it : dict_c)
    meesage_command(it.first);
}

void Console::input(void)
{
  while(1)
  {
    std::cout<<"->"<<std::flush;
    std::string line;
    std::getline(std::cin,line);
    eval(line);
  }
}
