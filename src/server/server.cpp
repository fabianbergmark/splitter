#include <iostream>

#include <memory>
#include <vector>
#include <utility>
#include <functional>

#include "config.hpp"
#include <splitter/network.hpp>
#include <splitter/sctp_server.hpp>
#include <splitter/tcp_connection.hpp>
#include <splitter/sctp_connection.hpp>

int forward(std::shared_ptr<Server> server
            , std::function<std::shared_ptr<TCP::Connection>()> connector) {

  std::vector<
    std::pair<
      std::shared_ptr<Connection>
      , std::shared_ptr<Connection>
      >> clients;

  fd_set sockets;
  char buffer[8192];

  while (true) {

    FD_ZERO(&sockets);
    FD_SET(server->get_socket(), &sockets);

    int maxfd = server->get_socket();

    for (auto &client : clients) {
      auto proxy = client.second->get_socket();
      auto connection = client.first->get_socket();

      FD_SET(proxy, &sockets);
      FD_SET(connection, &sockets);
      int max = proxy > connection
        ? proxy
        : connection;
      if (max > maxfd)
        maxfd = max;
    }

    if (select(maxfd+1, &sockets, NULL, NULL, NULL) != 0) {
      if (FD_ISSET(server->get_socket(), &sockets)) {
        int client = server->accept();
        std::shared_ptr<SCTP::Connection> connection(
          new SCTP::Connection(client));
        auto proxy = connector();
        clients.push_back(std::make_pair(connection, proxy));
      }
      for (size_t i = 0; i < clients.size(); ++i) {
        auto client     = clients[i];
        auto proxy      = client.second;
        auto connection = client.first;
        if (FD_ISSET(proxy->get_socket(), &sockets)) {
          try {
            int recieved = proxy->recv(buffer, sizeof(buffer));
            if (recieved > 0)
              int sent = connection->send(buffer, recieved);
          } catch (...) {
            clients.erase(clients.begin()+i);
            --i;
          }
        }
        if (FD_ISSET(connection->get_socket(), &sockets)) {
          try {
            int recieved = connection->recv(buffer, sizeof(buffer));
          if (recieved > 0)
            int sent     = proxy->send(buffer, recieved);
          } catch (...) {
            clients.erase(clients.begin()+i);
            --i;
          }
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {

  try {

    short port = config_port();

    short proxy_port = config_proxy_port();
    std::string proxy_hostname = config_proxy_hostname();

    std::shared_ptr<SCTP::Server> server(
      new SCTP::Server(port));

    auto connector = [&proxy_hostname, &proxy_port] ()
      -> std::shared_ptr<TCP::Connection> {
      return TCP::connect(proxy_hostname, proxy_port);
    };

    forward(server, connector);
    server->close();

  } catch (char const* error) {
    std::cerr << "Exception: " << error << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
  }
}
