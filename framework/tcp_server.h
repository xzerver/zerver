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
#ifndef __ZERVER_TCP_SERVER_H__
#define __ZERVER_TCP_SERVER_H__

#include "fsm.h"

namespace zerver {

class TcpServer {
  public:
    TcpServer(int num_threads, int port) : port_(port), 
    num_threads_(num_threads),
    acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)) {
    }

    void run();
  private:
    void start_accept();
    void on_accepted(TcpConnectionPtr conn, const boost::system::error_code& error);
    boost::asio::io_service io_service_;
    tcp::acceptor acceptor_;
    int port_;
    int num_threads_;
};

}

#endif
