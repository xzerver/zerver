#ifndef __PROXY_LOAD_BALANCER_H__
#define __PROXY_LOAD_BALANCER_H__

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "thirdparty/boost/thread/mutex.hpp"
#include "thirdparty/boost/thread/shared_mutex.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#define RECOVER_TIME 180

class LoadBalancer
{
public:
  typedef boost::shared_mutex ReadWriteMutex;
  typedef boost::shared_lock<ReadWriteMutex> ReadLock;
  typedef boost::unique_lock<ReadWriteMutex> WriteLock;
   
  LoadBalancer(boost::asio::io_service &io_service, const std::vector<std::string> &ips, uint16_t port)
  :port_(port),timer_(io_service)
  {
    ips_ = ips;
    std::vector<std::string>::const_iterator it;
    it =  ips_.begin();
    for(; it != ips_.end(); it++ ) {
      alive_ips_[*it] = 1;
    }
    call_later();
  }

  ~LoadBalancer()
  {

  }

  // alive_ips -> bad_ips -> good_ips -> alive_ips
  void clear_bad_ips();

  bool get_alive_ip_by_hash(int hash_val, std::string &ip, uint16_t &port) const;

  std::string get_ip_by_index(int index);

  bool get_alive_ip_random(std::string &ip, uint16_t &port);

  void server_connect_fail(const std::string &ip);

  protected:
    void call_later()
    {
      timer_.expires_from_now(boost::posix_time::milliseconds(RECOVER_TIME*1000));
      timer_.async_wait(boost::bind(&LoadBalancer::clear_bad_ips, this));
    }
  protected:
    ReadWriteMutex mutex_;
    uint16_t port_;
    std::vector<std::string> ips_;
    boost::unordered_set<std::string> bad_ips_;
    boost::unordered_set<std::string> good_ips_; 
    boost::unordered_map<std::string,int> alive_ips_;
    boost::asio::deadline_timer timer_;
};

#endif //__PROXY_LOAD_BALANCER_H__

