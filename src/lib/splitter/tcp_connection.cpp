#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include <memory>

#include "tcp_connection.hpp"

TCP::Connection::Connection(int socket)
  : socket(socket) {
}

void TCP::Connection::close() {
  ::close(this->socket);
}

int TCP::Connection::send(const char *buffer, unsigned int size) {
  int sent = ::send(socket, buffer, size, 0);
  if (sent == -1) {
    std::cerr << "Unable to send ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to send";
  }
  return sent;
}

int TCP::Connection::recv(char *buffer, unsigned int size) {
  int recieved = ::recv(this->socket, buffer, size, 0);
  if (recieved == -1) {
    std::cerr << "Unable to recieve ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to recieve";
  }
  return recieved;
}

int TCP::Connection::get_socket() {
  return this->socket;
}

std::shared_ptr<TCP::Connection> TCP::connect(const std::string &hostname, short port) {

  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock < 0) {
    std::cerr << "Unable to create socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to create socket";
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  addr.sin_family = AF_INET;
  inet_aton(hostname.c_str(), &addr.sin_addr);
  addr.sin_port = htons(port);

  if (connect(sock, (sockaddr*)&addr, sizeof(sockaddr)) < 0) {
    std::cerr << "Unable to connect ["
              << strerror(errno)
              << "]" << std::endl;
    close(sock);
    throw "Unable to connect";
  }

  return std::shared_ptr<TCP::Connection>(new TCP::Connection(sock));
}
