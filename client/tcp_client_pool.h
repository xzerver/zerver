// Copyright (C) 2014, 2015 Fanxi Zeng
// Contact: zengfancy@126.com 
// 
// This file is part of xzerver.
// 
// xzerver is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// xzerver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with xzerver. If not, see <http://www.gnu.org/licenses/>.
//
#ifndef __ZERVER_TCP_CLIENT_POOL_H__
#define __ZERVER_TCP_CLIENT_POOL_H__

#include "tcp_client.h"
#include "boost/unordered_map.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/shared_mutex.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace zerver {

class TcpClientPool
{
public:
  typedef boost::shared_mutex ReadWriteMutex;
  typedef boost::shared_lock<ReadWriteMutex> ReadLock;
  typedef boost::unique_lock<ReadWriteMutex> WriteLock;
  typedef std::deque<TcpClientPtr> TcpClientDeque;
  typedef boost::unordered_map<int, TcpClientDeque> TcpClientMap;

  TcpClientPool(int check_time, boost::asio::io_service &io_service, int times = 1000)
  :timer_(io_service)
  {
    check_time_ = check_time;
    max_times_ = times;
    call_later();
  }

  TcpClientPtr get_client(int peer_id);

  int size() const;

  std::string debug_string() const;

  void recollect(TcpClientPtr client);

  void check_clients();

protected:
  void call_later()
  {
    timer_.expires_from_now(boost::posix_time::milliseconds(check_time_ * 1000));
    timer_.async_wait(boost::bind(&TcpClientPool::check_clients, this));
  }

protected:
  int max_times_;
  int check_time_;
  ReadWriteMutex mutex_;
  TcpClientMap free_clients_;
  boost::asio::deadline_timer timer_;
};

}
#endif // __ZERVER_TCP_CLIENT_POOL_H__

