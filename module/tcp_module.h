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
#ifndef __ZERVER_TCP_MODULE_H__
#define __ZERVER_TCP_MODULE_H__

#include "../framework/module.h"
#include "../client/tcp_client.h"
#include "../client/tcp_client_pool.h"
#include <boost/shared_array.hpp>

namespace zerver {

class TcpModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        TcpClientPtr client_;
        boost::asio::deadline_timer timer_;
    };

    TcpModule(TcpClientPool& pool, const std::string& name) : Module(name), 
      client_pool_(pool) {
    }

    bool is_async() { return false; }
    virtual ModState run(FsmContextPtr context);
    virtual ModuleDataPtr create_data() { 
      return ModuleDataPtr(new ModuleData); 
    }

  private:
    void on_connect(FsmContextPtr context, 
        const boost::system::error_code& error);

    ModState send_request(FsmContextPtr context);

    void on_write_req(FsmContextPtr context, 
        const boost::system::error_code& error);

    void on_recv_resp_head(FsmContextPtr context,
        boost::shared_array<char> head, 
        uint32_t len, 
        const boost::system::error_code& error);

    void on_recv_resp_body(FsmContextPtr context,
        boost::shared_array<char> body, 
        uint32_t len, 
        const boost::system::error_code& error);

    void on_handle_time_out(FsmContextPtr context);

    void cancel_time_out_timer(FsmContextPtr context);

    virtual uint32_t get_time_out_ms() { return 50; }

    virtual uint32_t get_resp_head_len() { return 256; }
    virtual void on_send_req_failed(FsmContextPtr context) {}
    virtual void on_read_head_failed(FsmContextPtr context) {}
    virtual void on_read_body_failed(FsmContextPtr context) {}
    virtual void on_recv_resp_failed(FsmContextPtr context) {}
    virtual void on_connect_failed(FsmContextPtr context) {}
    virtual bool on_recv_resp_impl(boost::shared_array<char> body, 
        uint32_t len, 
        FsmContextPtr context) = 0;
    virtual void on_handle_time_out_impl(FsmContextPtr context);
    virtual uint32_t get_client_type_id(FsmContextPtr context) = 0;
    virtual void get_server_ip_port(std::string& ip, unsigned short& port, 
        FsmContextPtr context) = 0;

    virtual void get_request(char** req, uint32_t* len, FsmContextPtr context) = 0;
    virtual uint32_t get_resp_body_len(const char* head, uint32_t len, FsmContextPtr context) = 0;
    TcpClientPool& client_pool_;
};


}

#endif
