#ifndef DEFINE_CONFIG_HPP
#define DEFINE_CONFIG_HPP

#include <string>
#include <fstream>
#include <sstream>

template<class Type>
Type read_config(const std::string &filename) {
  std::string content;
  std::ifstream config(filename.c_str());
  if (!config)
    throw "Unable to read config";
  std::getline(config, content);
  std::stringstream convert(content.c_str());
  Type t;
  convert >> t;
  return t;
}

#endif
