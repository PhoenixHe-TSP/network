#include <iostream>
#include <set>
#include "connection.h"

class server
{
public:
  server(asio::io_service& io_service, char* port):
      _io_service(io_service),
      _acceptor(io_service, tcp::endpoint(tcp::v4(), std::atoi(port)))
  {
    new_session();
  }

private:
  class session: conn_base{
  public:
    session(asio::io_service& io_service, tcp::acceptor& acceptor, server& s):
        conn_base(io_service), _server(s)
    {
      accept(acceptor);
    }

  private:
    void handle_init()
    {
      _server.new_session();
      std::cout<< "start a session."<< std::endl;
      msg_t message(1024*1024, 87);
      write(message);
    }

    void handle_read(msg_t& message)
    {
    }

    void handle_write(msg_t& message) 
    {
      write(message); 
    }

    void handle_close() 
    {
      std::cout<<"a session ended. reason: "<< error_c()<< std::endl;
      _server.close_session(this); 
    }

    server& _server;
  };

  friend session;

  void new_session()
  {
    sessions.insert(new session(_io_service, _acceptor, *this));
  }

  void close_session(session* s)
  {
    sessions.erase(s);
    delete s;
  }
  
  asio::io_service& _io_service;
  std::set<session*> sessions;
  tcp::acceptor _acceptor;
};

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr<< "Usage: server <port>"<< std::endl;
    return 1;
  }
  try
  {
    asio::io_service ios;
    server s(ios, argv[1]);
    ios. run();
  } 
  catch ( std::exception& e )
  {
    std::cerr<< "exception: "<< e.what()<< std::endl;
  } 
  std::cerr<< "exit."<< std::endl;
}
