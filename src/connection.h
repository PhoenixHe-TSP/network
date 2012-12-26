#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <list>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace asio=boost::asio;
using boost::system::error_code;

class conn_base: 
    private boost::noncopyable
{
public:
  typedef std::vector<unsigned char> msg_t;
  uint64_t incoming, outgoing;

protected:
  conn_base(asio::io_service&);

  virtual void handle_init()=0;
  virtual void handle_read(msg_t&)=0;
  virtual void handle_write(msg_t&)=0;
  virtual void handle_close()=0;

  void connect(char* address, char* port);
  void accept(tcp::acceptor&);
  void write(msg_t&);
  const size_t write_queue_size();
  const error_code& error_c();

private:
  bool _handle_error(const error_code&);
  void _handle_init(const error_code&);
  void _next_read();
  void _handle_read_head(const error_code&);
  void _handle_read(const error_code&);
  void _next_write();
  void _handle_write(const error_code&);

  asio::io_service& _io_service;
  tcp::socket _socket;
  msg_t _msg_read;
  std::list<msg_t> _write_queue;
  uint32_t _msg_read_len, _msg_write_len, _op_cnt;
  error_code _error_c;
};

#endif
