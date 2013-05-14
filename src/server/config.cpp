#include "config.hpp"

short config_port() {
  return read_config<short>("conf/port");
}

short config_proxy_port() {
  return read_config<short>("conf/proxy_port");
}

std::string config_proxy_hostname() {
  return read_config<std::string>("conf/proxy_hostname");
}
