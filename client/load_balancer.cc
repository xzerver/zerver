#include "load_balancer.h"

void LoadBalancer::clear_bad_ips()
{
  boost::unordered_set<std::string>::iterator it;
  WriteLock wlock(mutex_);
  it = good_ips_.begin();
  for (; it != good_ips_.end(); it++) {
    alive_ips_[*it] = 1;
  } 
  good_ips_.swap(bad_ips_);
  bad_ips_.clear();
  call_later();
}

bool LoadBalancer::get_alive_ip_by_hash(int hash_val, std::string &ip, uint16_t &port) const
{
  ip.clear();
  ReadLock rlock(const_cast<ReadWriteMutex&>(mutex_));
  if (ips_.empty()) {
    return false;
  }
  int index = hash_val % ips_.size();
  boost::unordered_set<std::string>::iterator it;
  it = bad_ips_.find(ips_[index]);
  if(it != bad_ips_.end())
    return  false;
  else {
    ip = ips_[index];
    port = port_;
    return true; 
  }
}

std::string LoadBalancer::get_ip_by_index(int index)
{
  std::string ip;
  ReadLock rlock(mutex_);
  if ((size_t)index >= ips_.size()) {
    return ip;
  }
  ip = ips_.at(index);
  return ip;
}

bool LoadBalancer::get_alive_ip_random(std::string &ip, uint16_t &port)
{
  bool ret = false;
  ip.clear();
  ReadLock rlock(mutex_);
  if (alive_ips_.size() != 0) {
    int i = 0, n;
    n = rand() % alive_ips_.size();
    boost::unordered_map<std::string,int>::iterator it;
    it = alive_ips_.begin();
    for(; i < n; i++) {
      it++;
    }
    if(it != alive_ips_.end()) {
      ip = it->first;
      port = port_;
      ret = true;
    }
  }
  return ret; 
}

void LoadBalancer::server_connect_fail(const std::string &ip)
{
  WriteLock wlock(mutex_);
  alive_ips_.erase(ip);
  bad_ips_.insert(ip);
}

