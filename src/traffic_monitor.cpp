#include "traffic_monitor.h"

void traffic_monitor::start()
{
  timer_.async_wait(boost::bind(&traffic_monitor::work, this));
}

void traffic_monitor::work()
{
  timer_.expires_from_now(boost::posix_time::seconds(interval_));
  timer_.async_wait(boost::bind(&traffic_monitor::work, this));

  uint64_t now(scanner_);
  if (buffer_.empty())
  {
    buffer_.push_back(now);
    return;
  }
  double traffic(now-buffer_.front());
  traffic/= buffer_.size()*interval_;

  static const char table[4]= {' ', 'k', 'M', 'G'};
  const char* level(table);
  for (;traffic> 1024; ++level, traffic/= 1024);

  output_<< prefix_
         << std::setprecision(2)<< std::fixed << traffic
         << *level<< "B/s"<< std::endl;
  
  if (buffer_.full()) buffer_.pop_front();
  buffer_.push_back(now);
}
