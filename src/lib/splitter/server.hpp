#ifndef DEFINE_SERVER_HPP
#define DEFINE_SERVER_HPP

class Server {
public:
  virtual int accept() = 0;
  virtual void close() = 0;
  virtual int get_socket() = 0;
};

#endif
