#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include <memory>

#include "tcp_server.hpp"

TCP::Server::Server(short port)
  : port(port) {
  bind();
}

void TCP::Server::close() {
  ::close(this->socket);
}

int TCP::Server::accept() {
  sockaddr_in caddr;
  unsigned int len = sizeof(sockaddr_in);

  int client = ::accept(this->socket, (sockaddr*)&caddr, &len);
  if (client < 0) {
    std::cerr << "Unable to accept client ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to accept client";
  }
  return client;
}

int TCP::Server::get_socket() {
  return this->socket;
}

void TCP::Server::bind() {

  socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (socket < 0) {
    std::cerr << "Unable to create socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to create socket";
  }

  int reuse = 1;

  if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR
                 , &reuse, sizeof(int)) < 0) {
    std::cerr << "Unable set reuse port ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to set reuse port";
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (::bind(socket, (sockaddr*)&addr, sizeof(sockaddr)) < 0) {
    std::cerr << "Unable to bind on socket["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to bind on socket";
  }

  if (listen(socket, 10)) {
    std::cerr << "Unable to listen on socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to listen on socket";
  }
}
