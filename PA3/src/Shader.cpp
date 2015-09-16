#ifndef SHADER_CPP
#define SHADER_CPP
#include "Shader.h"
#include <fstream>
#include <streambuf>
using namespace std;
Shader::Shader(){
}

Shader::~Shader(){
}

string Shader::Load(const string filename) {
 string source;
 ifstream fin(filename.c_str(),ios::in);
 string line = "";
 while(!fin.eof()){
   getline(fin,line);
   source.append(line + '\n');
 }
 fin.close();
return source;
}

#endif

