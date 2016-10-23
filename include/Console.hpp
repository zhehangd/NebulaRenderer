#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <map>


// This class parses input from the user, or from a text file.
// Following the directives, it assigns variables and executes commands.

class Console
{
public:
  /// Register a variable to the dictionary.
  bool addVariable(std::string name,std::string value);
  /// Register a command to the dictionary.
  bool addCommand(std::string name,bool (*f)(Console&));
  
  //
  template<class T>
  bool getVariable(std::string name,T *val,int len=1,char delimiter=' ');

  // Evaluate one line of string (which should contain only one directive).
  bool eval(std::string line);
  // Evaluate lines in a file.
  void runfile(const char *filename);
  // Print out all content.
  void show(void);
  
private:
  
  void meesage_not_found(std::string key){std::cerr<<"\""<<key<<"\" not found."<<std::endl;}
  void meesage_has_existed(std::string key){std::cerr<<"\""<<key<<"\" has existed."<<std::endl;}
  void meesage_variable(std::string name,std::string value){std::cout<<name<<" = \""<<value<<"\""<<std::endl;}
  void meesage_command(std::string name){std::cout<<name<<std::endl;}
  

  std::map<std::string,std::string> dict_v;
  std::map<std::string,bool(*)(Console&)> dict_c;
};

template<class T>
bool Console::getVariable(std::string name,T *val,int len,char delimiter)
{
  auto fvar = dict_v.find(name);
  if (fvar == dict_v.end()){
    meesage_not_found(name); 
    return false;
  }
  
  auto value = fvar->second;
  std::istringstream ss( value );
  std::string element;
  
  for(int i=0;i<len;i++)
  {
    if(!std::getline(ss,element,delimiter)){
      for(;i<len;i++)
        val[i] = 0;
      return false;
    }
    std::istringstream( element ) >> val[i];
  }
  
  return true;
}

#endif