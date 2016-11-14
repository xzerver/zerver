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
#include "sstream"
#include "tcp_client_pool.h"

namespace zerver {

  TcpClientPtr TcpClientPool::get_client(int peer_id)
  {
    TcpClientPtr client;
    TcpClientMap::iterator it;
    WriteLock wlock(mutex_);
    it = free_clients_.find(peer_id);
    if (it != free_clients_.end()) {
      while (!it->second.empty())
      {
        client = it->second.front();
        it->second.pop_front();
        if (client && !client->is_closed())
        {
          if (max_times_ > 0) {
            if (client->increase_times() > max_times_)
              client->close();
            else {
              if (!client->is_error()) {
                return client;
              } else {
                F_LOG(INFO) << "client is closed ,peer_id:" << peer_id  << " remote_ip:" << client->remote_ip();
              }
            }
          }
          else {
            if (!client->is_error()) {
              return client;
            } else {
              F_LOG(INFO) << "client is closed ,peer_id:" << peer_id  << " remote_ip:" << client->remote_ip();
            }
          }
        }
      }
    }
    client.reset();
    return client;
  }

  int TcpClientPool::size() const
  {
    int num = 0;
    TcpClientMap::const_iterator it;
    ReadLock rlock(const_cast<ReadWriteMutex&>(mutex_));
    it = free_clients_.begin();
    while (it != free_clients_.end())
    {
      num += it->second.size();
      it++;
    }
    return num;
  }

  std::string TcpClientPool::debug_string() const
  {
    std::stringstream stream;
    TcpClientMap::const_iterator it;
    ReadLock rlock(const_cast<ReadWriteMutex&>(mutex_));
    it = free_clients_.begin();
    while (it != free_clients_.end())
    {
      stream << it->first << ":" << it->second.size() << " ";
      it++;
    }
    return stream.str();
  }

  void TcpClientPool::check_clients()
  {
    WriteLock wlock(mutex_);
    if (free_clients_.empty()) {
      call_later();
      return;
    }
    time_t now = time(NULL);
    TcpClientMap::iterator it;
    TcpClientDeque::iterator it2;
    it = free_clients_.begin();
    while (it != free_clients_.end())
    {
      TcpClientDeque &ref_deque = it->second;
      it2 = ref_deque.begin();
      while (it2 != ref_deque.end())
      {
        if ((*it2)->expired(now, check_time_)) {
          (*it2)->close();
          it2 = ref_deque.erase(it2);
        }
        else if ((*it2)->is_closed() || (*it2)->is_error()) {
          it2 = ref_deque.erase(it2);
        }
        else {
          it2++;
        }
      }
      it++;
    }
    call_later();
  }

  void TcpClientPool::recollect(TcpClientPtr client)
  {
    if (!client)
      return;
    WriteLock wlock(mutex_);
    client->on_recollect();
    int peer_id = client->get_peer_id();
    free_clients_[peer_id].push_back(client);
  }
}

