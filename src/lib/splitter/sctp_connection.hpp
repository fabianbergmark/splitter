#ifndef DEFINE_SCTP_CONNECTION_HPP
#define DEFINE_SCTP_CONNECTION_HPP

#include <memory>

#include "connection.hpp"

namespace SCTP {
  class Connection
    : public ::Connection {
  public:
    Connection(int);
    virtual ~Connection() {};
    virtual void close();
    virtual int send(const char*, unsigned int);
    virtual int recv(char*, unsigned int);
    virtual int get_socket();
  protected:
    virtual void handle_notification(const char*);
  private:
    int socket;
  };

  std::shared_ptr<SCTP::Connection> connect(const std::string&, short);
}

#endif
