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
#ifndef __ZERVER_FSM_DATA_H__
#define __ZERVER_FSM_DATA_H__

#include <sys/types.h>
#include <vector>
#include <map>
#include <string>
#include "boost/asio.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include "types_def.h"
#include "module.h"
#include "fsm.h"
#include "tcp_connection.h"


namespace zerver {

class IFsmData {
  public:
    IFsmData() {
      //P_LOG(INFO) << "IFsmData New:" << this;
    }

    virtual ~IFsmData() {
      //P_LOG(INFO) << "IFsmData Delete:" << this;
    }

    virtual void reset() = 0;
};


class IFsmDataFactory {
  public:
    virtual FsmDataPtr create_data() = 0;
    virtual ~IFsmDataFactory() {
    }
};

class FsmContext {
  public:
    FsmContext(TcpConnectionPtr conn, Fsm* fsm, boost::asio::io_service& io_service);
    virtual ~FsmContext()
    {}

    void init();
    Fsm* fsm()
    { return fsm_; }

    boost::asio::io_service& io_service()
    { return io_service_; }

    bool is_exit()
    { return exit_; }

    void exit()
    { exit_ = true; }

    bool recollectable();
    void reset();

    FsmDataPtr data()
    { return data_; }

    TcpConnectionPtr conn()
    { return conn_; }

    ModuleDataPtr get_module_data(const std::string& name)
    { return module_data_map_[name]; }

    uint32_t UUID() const
    { return uuid_; }

    static void register_fsm_data_factory(IFsmDataFactory* factory);

  private:
    bool exit_;
    Fsm* fsm_;
    TcpConnectionPtr conn_;
    boost::asio::io_service& io_service_;
    FsmDataPtr data_;
    std::map<std::string, ModuleDataPtr> module_data_map_;
    static IFsmDataFactory* fsm_data_factory_;
    uint32_t uuid_;
};

}

#endif

