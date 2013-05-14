#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>

#include "network.hpp"

void device_list() {
  char buffer[8192] = {0};
  int sock = socket(PF_INET, SOCK_DGRAM, 0);

  if (sock < 0) {
    throw "Unable to create socket";
  }

  ifconf ifc;
  ifc.ifc_len = sizeof(buffer);
  ifc.ifc_buf = buffer;

  if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
    throw "IOCTL error";
  }

  ifreq *ifr = ifc.ifc_req;

  int interfaces = ifc.ifc_len / sizeof(ifreq);

  for (unsigned int i = 0; i < interfaces; ++i) {
    ifreq *interface = &ifr[i];
    std::string name = interface->ifr_name;
    sockaddr *addr = &interface->ifr_addr;

    int addr_len = 0;

    switch (addr->sa_family) {
    case AF_INET:
      addr_len = sizeof(sockaddr_in);
      break;
    case AF_INET6:
      addr_len = sizeof(sockaddr_in6);
      break;
    }

    char hostname[NI_MAXHOST];

    getnameinfo(addr, addr_len, hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD);

    std::string ip = ip_address(addr);

    if (ioctl(sock, SIOCGIFHWADDR, interface) < 0) {
      throw "Unable to read MAC address";
    }

    sockaddr *hwaddr = &interface->ifr_hwaddr;
    std::string mac = mac_address(hwaddr);

    std::cout << "Found interface " << name << " " << ip << "@" << hostname << " #" << mac << std::endl;
  }
}

std::string ip_address(sockaddr *addr) {

  char address[INET6_ADDRSTRLEN];

  switch (addr->sa_family) {
  case AF_INET:
    inet_ntop(AF_INET, &(((sockaddr_in*) addr)->sin_addr)
              , address
              , sizeof(address));
    break;
  case AF_INET6:
    inet_ntop(AF_INET6, &(((sockaddr_in6*) addr)->sin6_addr)
              , address
              , sizeof(address));
    break;
  default:
    throw "Unknown address type";
  }
  return address;
}

std::string mac_address(sockaddr *hwaddr) {

  char mac[16];

  sprintf(mac
          ,"%02x:%02x:%02x:%02x:%02x:%02x"
          ,(unsigned char)hwaddr->sa_data[0]
          ,(unsigned char)hwaddr->sa_data[1]
          ,(unsigned char)hwaddr->sa_data[2]
          ,(unsigned char)hwaddr->sa_data[3]
          ,(unsigned char)hwaddr->sa_data[4]
          ,(unsigned char)hwaddr->sa_data[5]);

  return mac;
}
