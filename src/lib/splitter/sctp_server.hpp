#ifndef DEFINE_SCTP_SERVER_HPP
#define DEFINE_SCTP_SERVER_HPP

#include "server.hpp"

namespace SCTP {
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
