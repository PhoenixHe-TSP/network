#include <boost/array.hpp>
#include <boost/bind.hpp>
#include "connection.h"

using boost::bind;

conn_base::conn_base(asio::io_service& io_service):
    _io_service(io_service), _socket(io_service),
    incoming(0), outgoing(0), _op_cnt(0)
{
}

void conn_base::connect(char* address, char* port)
{
  if (_socket.is_open()) return;
  tcp::resolver resolver_(_io_service);
  asio::async_connect(_socket,
      resolver_.resolve(tcp::resolver::query(address, port)),
      bind(&conn_base::_handle_init, this, _1));
  ++_op_cnt;
}

void conn_base::accept(tcp::acceptor& acceptor)
{
  if (_socket.is_open()) return;
  acceptor.async_accept(_socket,
      bind(&conn_base::_handle_init, this, _1));
  ++_op_cnt;
}

void conn_base::write(msg_t& message)
{
  _write_queue.emplace_back();
  _write_queue.back().swap(message);
  if (_write_queue.size()== 1)
    _next_write();
}

const size_t conn_base::write_queue_size() 
{
  return _write_queue.size(); 
}

const error_code& conn_base::error_c()
{
  return _error_c;
}

bool conn_base::_handle_error(const error_code& error)
{
  if (!error) return 0;
  if (!_error_c) _error_c= error;
  if (_socket.is_open())
  {
    _socket.cancel();
    _socket.close();
  }
  if (_op_cnt) return 1;
  handle_close();
  return 1;
}

void conn_base::_handle_init(const error_code& error)
{
  --_op_cnt;
  if (error) { _handle_error(error); return; }
  handle_init();
  _next_read();
}

void conn_base::_next_read()
{
  asio::async_read(_socket, 
      asio::buffer(&_msg_read_len, sizeof(&_msg_read_len)),
      bind(&conn_base::_handle_read_head, this, _1));
  ++ _op_cnt;
}

void conn_base::_handle_read_head(const error_code& error)
{
  --_op_cnt;
  if (_handle_error(error)) return;
  _msg_read.resize(_msg_read_len);
  asio::async_read(_socket, asio::buffer(_msg_read),
      bind(&conn_base::_handle_read, this, _1));
  ++_op_cnt;
}

void conn_base::_handle_read(const error_code& error)
{
  --_op_cnt;
  if (_handle_error(error)) return;
  incoming+= _msg_read_len;
  handle_read(_msg_read);
  _next_read();
}

void conn_base::_next_write()
{
  boost::array<asio::mutable_buffer, 2> buf_seq;
  _msg_write_len= _write_queue.front().size();
  buf_seq[0]= asio::mutable_buffer(&_msg_write_len, sizeof(_msg_write_len));
  buf_seq[1]= asio::mutable_buffer(_write_queue.front().data(), _msg_write_len);

  asio::async_write(_socket, buf_seq,
      bind(&conn_base::_handle_write, this, _1));
  ++_op_cnt;
}

void conn_base::_handle_write(const error_code& error)
{
  --_op_cnt;
  if (_handle_error(error)) return;
  outgoing+= _msg_write_len;
  handle_write(_write_queue.front());
  _write_queue.pop_front();
  if (_write_queue.empty()) return;
  _next_write();
}
