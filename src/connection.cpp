#include <boost/array.hpp>
#include <boost/bind.hpp>
#include "connection.h"

using boost::bind;

conn_base::conn_base(asio::io_service& io_service):
    _io_service(io_service), _socket(io_service),
    incoming(0), outgoing(0)
{
}

void conn_base::handle_init(){}
void conn_base::handle_read(msg_t&){}
void conn_base::handle_write(msg_t&){}
void conn_base::handle_close(const error_code&){}

void conn_base::close(const error_code& error)
{
  if (_socket.is_open())
  {
    _socket.cancel();
    _socket.close();
  }
  _io_service.post(boost::bind(&conn_base::handle_close,
      this, error));
  return;
}

void conn_base::connect(char* address, char* port)
{
  if (_socket.is_open()) return;
  tcp::resolver resolver_(_io_service);
  asio::async_connect(_socket,
      resolver_.resolve(tcp::resolver::query(address, port)),
      bind(&conn_base::_handle_init, this, _1));
}

void conn_base::accept(tcp::acceptor& acceptor)
{
  if (_socket.is_open()) return;
  acceptor.async_accept(_socket,
      bind(&conn_base::_handle_init, this, _1));
}

void conn_base::write(msg_t& message)
{
  _write_queue.emplace_back();
  _write_queue.back().swap(message);
  if (_write_queue.size()== 1)
    _next_write();
}

inline const size_t conn_base::write_queue_size() {
  return _write_queue.size(); }

void conn_base::_handle_init(const error_code& error)
{
  if (error) 
  { 
    close(error); 
    return; 
  }
  handle_init();
  _next_read();
}

void conn_base::_next_read()
{
  asio::async_read(_socket, 
      asio::buffer(&_msg_read_len, sizeof(&_msg_read_len)),
      bind(&conn_base::_handle_read_head, this, _1));
}

void conn_base::_handle_read_head(const error_code& error)
{
  if (error)
  { 
    close(error);
    return; 
  }
  _msg_read.resize(_msg_read_len);
  asio::async_read(_socket, asio::buffer(_msg_read),
      bind(&conn_base::_handle_read, this, _1));
}

void conn_base::_handle_read(const error_code& error)
{
  if (error) 
  { 
    close(error); 
    return; 
  }
  incoming+= _msg_read_len;
  handle_read(_msg_read);
  _next_read();
}

void conn_base::_next_write()
{
  static boost::array<asio::mutable_buffer, 2> buf_seq;

  _msg_write_len= _write_queue.front().size();
  buf_seq[0]= asio::mutable_buffer(&_msg_write_len, sizeof(_msg_write_len));
  buf_seq[1]= asio::mutable_buffer(_write_queue.front().data(), _msg_write_len);

  asio::async_write(_socket, buf_seq,
      bind(&conn_base::_handle_write, this, _1));
}

void conn_base::_handle_write(const error_code& error)
{
  if (error) 
  { 
    close(error); 
    return; 
  }
  outgoing+= _msg_write_len;
  handle_write(_write_queue.front());
  _write_queue.pop_front();
  if (_write_queue.empty()) return;
  _next_write();
}
