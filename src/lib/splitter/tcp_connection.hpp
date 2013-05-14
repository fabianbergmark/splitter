#ifndef DEFINE_TCP_HPP
#define DEFINE_TCP_HPP

#include <memory>
#include <string>

#include "connection.hpp"

namespace TCP {
  class Connection
    : public ::Connection {
  public:
    Connection(int);
    virtual void close();
    virtual int get_socket();
    virtual int recv(char*, unsigned int);
    virtual int send(const char*, unsigned int);
  private:
    int socket;
  };

  std::shared_ptr<Connection> connect(const std::string&, short);
}

#endif
