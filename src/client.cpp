#include "connection.h"
#include "traffic_monitor.h"

class client: conn_base{
public:
  client( asio::io_service& io_service, char* address, char* port):
      conn_base(io_service), 
      m0(io_service, incoming, "Incoming:"), 
      m1(io_service, outgoing, "Outgoing:")
  {
    connect(address, port);
  }
  
private:
  void handle_init()
  {
    std::cout<< "connection started."<< std::endl;
    m0.start();
    m1.start();
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
    auto error(error_c());
    throw error;
  }

  traffic_monitor m0, m1;
};

int main(int argc, char* argv[]){
  if (argc != 3)
  {
    std::cerr<< "Usage: client <address> <port>"<< std::endl;
    return 1;
  }
  try
  {
    asio::io_service ios;
    client c( ios, argv[1], argv[2]);
    std::cout<< sizeof(c)<< std::endl;
    ios. run();
  } 
  catch ( std::exception& e )
  {
    std::cerr<< "exception: "<< e.what()<< std::endl;
  }
  catch ( const boost::system::error_code& error )
  {
    std::cerr<< "an error occured." << error<< std::endl;
  }
  std::cerr<< "exit."<< std::endl;
}
