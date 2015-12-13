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
#include <deque>

namespace zerver {

class TcpClientPool {
  public:
    TcpClientPtr get_client(uint32_t client_type_id);
    void recollect(TcpClientPtr client);

  protected:
    void _lock();
    void _unlock();

  protected:
    typedef std::map<uint32_t, std::deque<TcpClientPtr> > ClientMap;
    ClientMap client_map_;
};
}


#endif
