#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>

#include <cstring>
#include <iostream>

#include <memory>

#include "sctp_server.hpp"

SCTP::Server::Server(short port)
  : port(port) {
  bind();
}

int SCTP::Server::accept() {
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

void SCTP::Server::close() {
  ::close(this->socket);
}

int SCTP::Server::get_socket() {
  return socket;
}

void SCTP::Server::bind() {

  socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

  if (socket < 0) {
    std::cerr << "Unable to create socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to create socketet";
  }

  sctp_paddrparams heartbeat;
  memset(&heartbeat, 0, sizeof(sctp_paddrparams));

  heartbeat.spp_flags = SPP_HB_ENABLE;
  heartbeat.spp_hbinterval = 1000;
  heartbeat.spp_pathmaxrxt = 1;

  if (setsockopt(socket, SOL_SCTP, SCTP_PEER_ADDR_PARAMS
                 , &heartbeat, sizeof(sctp_paddrparams)) != 0) {
    std::cerr << "Unable to modify SCTP heartbeat option ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to modify SCTP heartbeat option";
  }

  sctp_event_subscribe event;
  memset(&event, 1, sizeof(sctp_event_subscribe));

  if (setsockopt(socket, IPPROTO_SCTP, SCTP_EVENTS
                 , &event, sizeof(sctp_event_subscribe)) != 0) {
    std::cerr << "Unable to subscribe to SCTP events ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to subscribe to SCTP events";
  }

  int reuse = 1;

  if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR
                 , &reuse, sizeof(int))) {
    std::cerr << "Unable to set reuse port option ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to set reuse port option";
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (::bind(this->socket, (sockaddr*)&addr, sizeof(sockaddr)) < 0) {
    std::cerr << "Unable to bind on socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to bind on socket";
  }

  if (listen(this->socket, 100) < 0) {
    std::cerr << "Unable to listen on socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to listen on socket";
  }
}
