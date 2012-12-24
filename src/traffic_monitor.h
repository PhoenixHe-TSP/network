#ifndef _TRAFFIC_MONITOR_H_
#define _TRAFFIC_MONITOR_H_

#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class traffic_monitor{
public:
  traffic_monitor(boost::asio::io_service& io_service, 
      const uint64_t& scanner,
      std::string prefix= "traffic: ",
      std::ostream& output= std::cout,
      size_t interval= 1, size_t loop= 10 ):
        scanner_(scanner),
        interval_(interval), output_(output),
        timer_(io_service, boost::posix_time::seconds(interval_)), 
        prefix_(prefix), buffer_(loop)
  {
  }

  void start();
private:
  void work();

  const uint64_t& scanner_;
  std::string prefix_;
  std::ostream& output_;
  size_t interval_;
  boost::asio::deadline_timer timer_;
  boost::circular_buffer<uint64_t> buffer_;
};

#endif
