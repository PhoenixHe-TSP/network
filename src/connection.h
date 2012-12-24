#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <list>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace asio=boost::asio;
using boost::system::error_code;

class conn_base: boost::noncopyable
{
public:
  typedef std::vector<unsigned char> msg_t;
  uint64_t incoming, outgoing;

protected:
  conn_base(asio::io_service&);

  virtual void handle_init();
  virtual void handle_read(msg_t&);
  virtual void handle_write(msg_t&);
  virtual void handle_close(const error_code&);

  void close(const error_code&);
  void connect(char* address, char* port);
  void accept(tcp::acceptor&);
  void write(msg_t&);
  const size_t write_queue_size();

private:
  void _handle_init(const error_code&);
  void _next_read();
  void _handle_read_head(const error_code&);
  void _handle_read(const error_code&);
  void _next_write();
  void _handle_write(const error_code&);

  asio::io_service& _io_service;
  tcp::socket _socket;
  msg_t _msg_read;
  uint32_t _msg_read_len, _msg_write_len;
  std::list<msg_t> _write_queue;
};

#endif
