#include <iostream>

#include <vector>

#include <splitter/sctp_server.hpp>
#include <splitter/sctp_connection.hpp>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cout << "Usage: echo port" << std::endl;
    return 0;
  }

  short port = atoi(argv[1]);

  try {

    auto server = SCTP::Server(port);
    std::vector<std::shared_ptr<Connection>> clients;

    fd_set sockets;
    char buffer[8192];

    while (true) {

      int socket = server.get_socket();
      int maxfd = socket;
      FD_SET(socket, &sockets);

      for (auto &client : clients) {
        auto socket = client->get_socket();
        FD_SET(socket, &sockets);
        if (socket > maxfd)
          maxfd = socket;
      }

      if (select(maxfd+1, &sockets, NULL, NULL, NULL) != 0) {
        if (FD_ISSET(socket, &sockets)) {
          std::cout << "New client connected" << std::endl;
          int client = server.accept();
          std::shared_ptr<SCTP::Connection> connection(
            new SCTP::Connection(client));
          clients.push_back(connection);
        }
        for (size_t i = 0; i < clients.size(); ++i) {
          auto &client = clients[i];
          if (FD_ISSET(client->get_socket(), &sockets)) {
            try {
              int recieved = client->recv(buffer, sizeof(buffer));
              if (recieved > 0) {
                std::cout << "Echoing '" << buffer << "'" << std::endl;
                int sent = client->send(buffer, recieved);
              }
            } catch (...) {
              clients.erase(clients.begin()+i);
              --i;
            }
          }
        }
      }
    }
  } catch (...) {
    std::cerr << "Unable to bind on port" << std::endl;
    return 0;
  }
}
