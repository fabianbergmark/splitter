#ifndef DEFINE_NETWORK_HPP
#define DEFINE_NETWORK_HPP

#include <string>

#include <sys/socket.h>

void device_list();
std::string ip_address(sockaddr*);
std::string mac_address(sockaddr*);

#endif
