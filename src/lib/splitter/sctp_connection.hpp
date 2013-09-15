#ifndef DEFINE_SCTP_CONNECTION_HPP
#define DEFINE_SCTP_CONNECTION_HPP

#include <arpa/inet.h>
#include <netinet/sctp.h>

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
    virtual void handle_notification(const char*, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_assoc_change(sctp_assoc_change&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_paddr_change(sctp_paddr_change&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_remote_error(sctp_remote_error&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_send_failed(sctp_send_failed&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_shutdown(sctp_shutdown_event&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_partial_delivery(sctp_pdapi_event&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_sender_dry(sctp_sender_dry_event&, sockaddr_in&, sctp_sndrcvinfo&);
    virtual void handle_notification_stopped(sctp_notification&, sockaddr_in&, sctp_sndrcvinfo&);
  private:
    int socket;
  };

  std::shared_ptr<SCTP::Connection> connect(const std::string&, short);
}

#endif
