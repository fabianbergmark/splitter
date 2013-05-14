#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <cstdlib>

#include <splitter/sctp_connection.hpp>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cout << "Usage: nc hostname port" << std::endl;
    return 0;
  }

  std::string hostname(argv[1]);
  short port = atoi(argv[2]);

  try {

    auto connection = SCTP::connect(hostname, port);

    auto fdin   = fileno(stdin);
    auto socket = connection->get_socket();

    fd_set sockets;
    char buffer[8192];

    while (true) {

      int maxfd = socket > fdin ? socket : 0;
      FD_SET(socket, &sockets);
      FD_SET(fdin, &sockets);

      if (select(maxfd+1, &sockets, NULL, NULL, NULL) != 0) {
        if (FD_ISSET(socket, &sockets)) {
          try {
            int recieved = connection->recv(buffer, sizeof(buffer));
            if (recieved > 0) {
              buffer[recieved] = '\0';
              std::cout << buffer << std::endl;
            }
          } catch (...) {
            std::cerr << "Server disconnected" << std::endl;
            return 0;
          }
        }
        if (FD_ISSET(0, &sockets)) {
          int recieved = read(fdin, buffer, sizeof(buffer));
          if (recieved > 0) {
            buffer[recieved-1] = '\0';
            try {
              connection->send(buffer, recieved);
            } catch (...) {
              std::cerr << "Server disconnected" << std::endl;
              return 0;
            }
          }
        }
      }
    }
  } catch (...) {
    std::cerr << "Unable to connect" << std::endl;
    return 0;
  }
}
