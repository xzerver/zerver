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
#ifndef __ZERVER_TCP_CLIENT_H__
#define __ZERVER_TCP_CLIENT_H__

#include "../framework/types_def.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace zerver {
class TcpClient {
  public:
    TcpClient(boost::asio::io_service& io_service, uint32_t client_type_id) : 
      socket_(io_service),
      client_type_id_(client_type_id) {
    }

    tcp::socket& socket() { return socket_; }
    uint32_t client_type_id() { return client_type_id_; }

  private:
    tcp::socket socket_;
    uint32_t client_type_id_;
};

typedef boost::shared_ptr<TcpClient> TcpClientPtr;
}
#endif
