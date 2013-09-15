#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>

#include <cstring>
#include <iostream>

#include <memory>

#include "network.hpp"
#include "sctp_connection.hpp"

SCTP::Connection::Connection(int socket)
  : socket(socket) {
}

void SCTP::Connection::close() {
  ::close(this->socket);
}

int SCTP::Connection::send(const char *buffer, unsigned int size) {
  int sent = sctp_sendmsg(this->socket, buffer, size
                          , NULL, 0, 0, 0, 0, 0, 0);
  if (sent == -1) {
    std::cerr << "Unable to send ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to send";
  }
  return sent;
}

int SCTP::Connection::recv(char *buffer, unsigned int size) {
  int flags = 0;
  sockaddr_in saddr;
  unsigned int len = sizeof(sockaddr_in);
  sctp_sndrcvinfo rcvinfo;

  int recieved = sctp_recvmsg(this->socket, buffer, size
                              , (sockaddr*)&saddr, &len
                              , &rcvinfo, &flags);

  if (flags & MSG_NOTIFICATION) {
    handle_notification(buffer, saddr, rcvinfo);
    return 0;
  } else {
    if (recieved <= 0) {
      std::cerr << "Unable to recieve ["
                << strerror(errno)
                << "]" << std::endl;
      throw "Unable to recieve";
    } else
      return recieved;
  }
}

int SCTP::Connection::get_socket() {
  return this->socket;
}

void SCTP::Connection::handle_notification(const char *buffer, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {

  sctp_notification notification;
  memcpy(&notification, buffer, sizeof(sctp_notification));

  auto tlv = notification.sn_header;
  switch (tlv.sn_type) {
  case SCTP_ASSOC_CHANGE: {
    sctp_assoc_change assoc = notification.sn_assoc_change;
    handle_notification_assoc_change(assoc, saddr, rcvinfo);
    break;
  }
  case SCTP_PEER_ADDR_CHANGE: {
    sctp_paddr_change paddr = notification.sn_paddr_change;
    handle_notification_paddr_change(paddr, saddr, rcvinfo);
    break;
  }
  case SCTP_REMOTE_ERROR: {
    sctp_remote_error error = notification.sn_remote_error;
    handle_notification_remote_error(error, saddr, rcvinfo);
    break;
  }
  case SCTP_SEND_FAILED: {
    sctp_send_failed fail = notification.sn_send_failed;
    handle_notification_send_failed(fail, saddr, rcvinfo);
    break;
  }
  case SCTP_SHUTDOWN_EVENT: {
    sctp_shutdown_event shutdown = notification.sn_shutdown_event;
    handle_notification_shutdown(shutdown, saddr, rcvinfo);
    break;
  }
  case SCTP_PARTIAL_DELIVERY_EVENT: {
    sctp_pdapi_event pdapi = notification.sn_pdapi_event;
    handle_notification_partial_delivery(pdapi, saddr, rcvinfo);
    break;
  }
  }
}

void SCTP::Connection::handle_notification_assoc_change(sctp_assoc_change &assoc, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Association change event from "
            << ip_address((sockaddr*)&saddr);
  switch (assoc.sac_state) {
  case SCTP_COMM_UP:
    std::cerr << " [SCTP_COMM_UP]"
              << " inbound streams: (" << assoc.sac_inbound_streams << ")"
              << " outbound streams: (" << assoc.sac_outbound_streams << ")";
    break;
  case SCTP_COMM_LOST:
    std::cerr << " [SCTP_COMM_LOST]"
              << " error: (" <<  strerror(assoc.sac_error) << ")";
    break;
  case SCTP_RESTART:
    std::cerr << " [SCTP_RESTART]";
    break;
  case SCTP_SHUTDOWN_COMP:
    std::cerr << " [SCTP_SHUTDOWN_COMP]";
    break;
  case SCTP_CANT_STR_ASSOC:
    std::cerr << " [SCTP_CANT_STR_ASSOC]"
              << " error: (" << strerror(assoc.sac_error) << ")";
    break;
  default:
    break;
  }
  std::cerr << std::endl;
}

void SCTP::Connection::handle_notification_paddr_change(sctp_paddr_change &paddr, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Peer address change event from "
            << ip_address((sockaddr*)&saddr);
  switch (paddr.spc_state) {
  case SCTP_ADDR_AVAILABLE:
    std::cerr << " [SCTP_ADDR_AVAILABLE]";
    break;
  case SCTP_ADDR_UNREACHABLE:
    std::cerr << " [SCTP_ADDR_AVAILABLE]";
    break;
  case SCTP_ADDR_REMOVED:
    std::cerr << " [SCTP_ADDR_REMOVED]";
    break;
  case SCTP_ADDR_ADDED:
    std::cerr << " [SCTP_ADDR_ADDED]";
    break;
  case SCTP_ADDR_MADE_PRIM:
    std::cerr << " [SCTP_ADDR_MADE_PRIM]";
    break;
  }
  std::cerr << " address: (" << ip_address((sockaddr*)&paddr.spc_aaddr) << ")"
            << std::endl;
}

void SCTP::Connection::handle_notification_remote_error(sctp_remote_error &error, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Remote error event from "
            << ip_address((sockaddr*)&saddr);
  std::cerr << std::endl;
}

void SCTP::Connection::handle_notification_send_failed(sctp_send_failed &fail, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Send failed event to "
            << ip_address((sockaddr*)&saddr);
  switch (fail.ssf_flags) {
  case SCTP_DATA_UNSENT:
    std::cerr << " [SCTP_DATA_UNSENT]";
    break;
  case SCTP_DATA_SENT:
    std::cerr << " [SCTP_DATA_SENT]";
  }
  std::cerr << " error: (" << fail.ssf_error << ")"
            << std::endl;
}

void SCTP::Connection::handle_notification_shutdown(sctp_shutdown_event &shutdown, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Shutdown event from "
            << ip_address((sockaddr*)&saddr);
  std::cerr << std::endl;
}

void SCTP::Connection::handle_notification_partial_delivery(sctp_pdapi_event &pdapi, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Partial delivery event from "
            << ip_address((sockaddr*)&saddr);
  switch (pdapi.pdapi_indication) {
  case SCTP_PARTIAL_DELIVERY_ABORTED:
    std::cerr << " [SCTP_PARTIAL_DELIVIERY_ABORTED]";
    break;
  }
  std::cerr << std::endl;
}

void SCTP::Connection::handle_notification_sender_dry(sctp_sender_dry_event &sender_dry, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Sender dry event from "
            << ip_address((sockaddr*)&saddr);
  std::cerr << std::endl;
}

void SCTP::Connection::handle_notification_stopped(sctp_notification &notifiaction, sockaddr_in &saddr, sctp_sndrcvinfo &rcvinfo) {
  std::cerr << "Notification stopped event "
            << ip_address((sockaddr*)&saddr);
  std::cerr << std::endl;
}

std::shared_ptr<SCTP::Connection> SCTP::connect(const std::string &hostname, short port) {

  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

  if (sock < 0) {
    std::cerr << "Unable to create socket ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to create socket";
  }

  sctp_paddrparams heartbeat;
  memset(&heartbeat, 0, sizeof(sctp_paddrparams));

  heartbeat.spp_flags = SPP_HB_ENABLE;
  heartbeat.spp_hbinterval = 1000;
  heartbeat.spp_pathmaxrxt = 1;

  if (setsockopt(sock, SOL_SCTP, SCTP_PEER_ADDR_PARAMS
                 , &heartbeat, sizeof(sctp_paddrparams)) != 0) {
    std::cerr << "Unable to modify SCTP hearbeat options ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to modify SCTP heartbeat options";
  }

  sctp_initmsg initmsg;
  memset(&initmsg, 0, sizeof(sctp_initmsg));

  initmsg.sinit_num_ostreams = 2;
  initmsg.sinit_max_instreams = 2;
  initmsg.sinit_max_attempts = 1;

  if (setsockopt(sock, SOL_SCTP, SCTP_INITMSG
                 , &initmsg, sizeof(sctp_initmsg)) != 0) {
    std::cerr << "Unable to set SCTP init message ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to set SCTP init message";
  }

  sctp_event_subscribe event;
  memset(&event, 1, sizeof(sctp_event_subscribe));

  if (setsockopt(sock, IPPROTO_SCTP, SCTP_EVENTS
                 , &event, sizeof(sctp_event_subscribe)) != 0) {
    std::cerr << "Unable to subscribe to SCTP events ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to subscribe to SCTP events";
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  addr.sin_family = AF_INET;
  inet_aton(hostname.c_str(), &addr.sin_addr);
  addr.sin_port = htons(port);

  if (connect(sock, (sockaddr*)&addr, sizeof(sockaddr)) < 0) {
    close(sock);
    std::cerr << "Unable to connect ["
              << strerror(errno)
              << "]" << std::endl;
    throw "Unable to connect";
  }

  return std::shared_ptr<Connection>(new SCTP::Connection(sock));
}
