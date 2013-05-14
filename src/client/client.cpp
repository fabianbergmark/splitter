#include <iostream>

#include <memory>
#include <vector>
#include <utility>
#include <functional>

#include "config.hpp"
#include <splitter/network.hpp>
#include <splitter/tcp_server.hpp>
#include <splitter/tcp_connection.hpp>
#include <splitter/sctp_connection.hpp>

void forward(std::shared_ptr<Server> server
             , std::function<std::shared_ptr<Connection>()> connector) {

  std::vector<
    std::pair<
      std::shared_ptr<Connection>
      , std::shared_ptr<Connection>
      >> clients;

  fd_set sockets;
  char buffer[8192];

  while (true) {
    FD_ZERO(&sockets);

    int maxfd = server->get_socket();
    FD_SET(server->get_socket(), &sockets);

    for (auto &client : clients) {
      auto proxy = client.second->get_socket();
      auto browser = client.first->get_socket();

      FD_SET(proxy, &sockets);
      FD_SET(browser, &sockets);
      int max = browser > proxy ? browser : proxy;
      if (max > maxfd)
        maxfd = max;
    }

    if (select(maxfd+1, &sockets, NULL, NULL, NULL) != 0) {
      if (FD_ISSET(server->get_socket(), &sockets)) {
        int client = server->accept();
        std::shared_ptr<SCTP::Connection> browser(
          new SCTP::Connection(client));
        auto connection = connector();
        clients.push_back(std::make_pair(browser, connection));
      }
      for (size_t i = 0; i < clients.size(); ++i) {
        auto client     = clients[i];
        auto browser    = client.first;
        auto connection = client.second;

        if (FD_ISSET(connection->get_socket(), &sockets)) {
          try {
          int recieved = connection->recv(buffer, sizeof(buffer));
          if (recieved > 0)
            int sent = browser->send(buffer, recieved);
          } catch (...) {
            clients.erase(clients.begin()+i);
            --i;
          }
        }
        if (FD_ISSET(browser->get_socket(), &sockets)) {
          try {
          int recieved = browser->recv(buffer, sizeof(buffer));
          if (recieved > 0)
            int sent = connection->send(buffer, recieved);
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

  short port = config_port();

  short server_port = config_server_port();
  std::string server_hostname = config_server_hostname();

  //device_list();

  try {

    std::shared_ptr<TCP::Server> server(new TCP::Server(port));

    auto connector = [&server_hostname, &server_port] ()
      -> std::shared_ptr<SCTP::Connection> {
      return SCTP::connect(server_hostname, server_port);
    };

    forward(server, connector);

    server->close();

  } catch(char const *error) {
    std::cerr << "Exception: " << error << std::endl;
  }
}
