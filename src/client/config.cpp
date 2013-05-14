#include "config.hpp"

short config_port() {
  return read_config<short>("conf/port");
}

short config_server_port() {
  return read_config<short>("conf/server_port");
}

std::string config_server_hostname() {
  return read_config<std::string>("conf/server_hostname");
}
