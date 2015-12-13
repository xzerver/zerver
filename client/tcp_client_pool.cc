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
#include "tcp_client_pool.h"


namespace zerver {

TcpClientPtr TcpClientPool::get_client(uint32_t client_type_id) {
  _lock();
  ClientMap::iterator it = client_map_.find(client_type_id);
  if (it != client_map_.end()) {
    if (!it->second.empty()) {
      TcpClientPtr client = it->second.back();
      // pop from the back
      it->second.pop_back();
      return client;
    } else {
      return TcpClientPtr();
    }
  } else {
    return TcpClientPtr();
  }

  _unlock();
}

void TcpClientPool::recollect(TcpClientPtr client) {
  _lock();

  uint32_t client_type_id = client->client_type_id();
  ClientMap::iterator it = client_map_.find(client_type_id);
  if (it != client_map_.end()) {
    // push from the front
    it->second.push_front(client);
  } else {
    std::deque<TcpClientPtr> q;
    q.push_front(client);
    client_map_[client_type_id] = q;
  }

  _unlock();
}

}
