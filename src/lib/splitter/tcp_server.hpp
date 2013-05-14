#ifndef DEFINE_TCP_SERVER_HPP
#define DEFINE_TCP_SERVER_HPP

#include "server.hpp"

namespace TCP {
  class Server
    : public ::Server {
  public:
    Server(short);
    virtual int accept();
    virtual void close();
    virtual int get_socket();
  protected:
    void bind();
  private:
    int socket;
    short port;
  };
}

#endif
