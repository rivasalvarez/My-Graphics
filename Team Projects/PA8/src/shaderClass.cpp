#ifndef SHADERSRCLOAD_CPP
#define SHADERSRCLOAD_CPP

#include <fstream>
#include "shaderClass.h"




std::string ShaderSrcLoader::readShader(std::string arg)
   {
      std::string shaderSrc;
      std::ifstream fin;
      std::string temp;

      fin.open(arg.c_str());
      
      while(!fin.eof())       
      {
         getline(fin,temp);

         shaderSrc.append(temp + '\n');                
      }

      fin.close();
return shaderSrc;
   } 

#endif
