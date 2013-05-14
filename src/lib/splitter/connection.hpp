#ifndef DEFINE_CONNECTION_HPP
#define DEFINE_CONNECTION_HPP

class Connection {
public:
  virtual ~Connection() {};
  virtual void close() = 0;
  virtual int get_socket() = 0;
  virtual int recv(char*, unsigned int) = 0;
  virtual int send(const char*, unsigned int) = 0;
};

#endif
