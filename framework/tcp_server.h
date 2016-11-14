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
#include "thirdparty/boost/shared_ptr.hpp"
namespace zerver {
enum run_mode
{
  RUN_NULL = 0x0,
  RUN_THREAD = 0x10,
  RUN_PROCESS,
};

class TcpServer {
  public:
    TcpServer(int port, int task_number);

    virtual ~TcpServer();

    boost::asio::io_service& io_service()
    {
      return *io_service_;
    }

    void run(run_mode mod = RUN_THREAD);

    boost::shared_ptr<Fsm> fsm()
    {
      return fsm_;
    }

  private:
    void start_accept();
    void start_signal_wait();
    void run_for_child(int thread_num = 1);
    void run_multi_thread(int thread_num = 1);
    void run_multi_process(int process_num = 1);
    void handle_signal(const boost::system::error_code& error, int signal_number);
    void on_accepted(TcpConnectionPtr conn, const boost::system::error_code& error);
  
  protected:
    virtual void clear();
  
    virtual void on_child_running(){}

    virtual void on_child_killing(boost::system::error_code err){}

  private:
    int port_;
    int task_number_;
    run_mode server_mode_;
    boost::shared_ptr<Fsm> fsm_;
    boost::shared_ptr<boost::asio::io_service> io_service_;
    boost::shared_ptr<boost::asio::signal_set> signal_;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};

}

#endif
