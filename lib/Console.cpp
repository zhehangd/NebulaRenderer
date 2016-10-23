#include "Console.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>

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


void eval_trim_blank(std::string &line)
{
  const char *blank = " \t\n\r\f\v";
  line.erase(0, line.find_first_not_of(blank));
  line.erase(line.find_last_not_of(blank)+1);
}

void eval_trim_comment(std::string &line)
{
  size_t pos = line.find_first_of("#");
  if (pos != std::string::npos)
    line = line.erase(pos);
}

// Parse an variable assignment expression.
bool eval_variable(const std::string &line,std::string &name,std::string &value)
{
  size_t eq = line.find_first_of("=");
  if (eq == std::string::npos)
    return false;
  name  = line.substr(0,eq); eval_trim_blank(name);
  value = line.substr(eq+1); eval_trim_blank(value);
}

bool eval_command(const std::string &line,std::string &name)
{
  name = line;
}

// Evaluate one line of string (which should contain only one directive).
bool Console::eval(std::string line)
{
  // Trim the string.
  eval_trim_blank(  line);
  eval_trim_comment(line);
  if (line.empty())
    return true;
  
  std::string name,value;
  
  // Variable
  if (eval_variable(line,name,value))
  {
    auto fvar = dict_v.find(name);
    if ( fvar == dict_v.end() ){
      meesage_not_found(name);
      return false;
    }
    fvar->second = value;
    return true;
  }

  // Command
  {
  eval_command(line,name);
  auto fcmd = dict_c.find(line);
  if( fcmd != dict_c.end() )
    return fcmd->second(*this);
  }
  
  // Variable Query
  {
    auto fvar = dict_v.find(line);
    if (fvar == dict_v.end()){
      meesage_not_found(name); 
      return false;
    }
    meesage_variable(fvar->first,fvar->second);
    return true;
  }
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


