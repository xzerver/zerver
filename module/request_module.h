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
#ifndef __ZERVER_REQUEST_MODULE_H__
#define __ZERVER_REQUEST_MODULE_H__

#include "framework/module.h"

namespace zerver {

class RequestModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        ModuleData(FsmContext* context) : timer_(context->io_service()) {
        }
        boost::asio::deadline_timer timer_;
    };

    RequestModule(const std::string& name) : Module(name) {
    }

    virtual ModState run(FsmContextPtr context, ModState last_mod_state);

    virtual ModuleDataPtr create_data(FsmContext* context) { 
      return ModuleDataPtr(new ModuleData(context)); 
    }
  protected:
    virtual uint32_t get_version_len(FsmContextPtr context) = 0;
    virtual uint32_t get_head_len(FsmContextPtr context) = 0;
    // return -1 if head illegal
    virtual uint32_t get_body_len(FsmContextPtr context) = 0;
    virtual uint32_t get_read_version_time_out_ms();

    virtual uint32_t get_read_head_and_body_time_out_ms();

    virtual void on_read_version_failed(FsmContextPtr context) {}
    virtual void on_read_head_failed(FsmContextPtr context) {}
    virtual void on_read_body_failed(FsmContextPtr context) {}
    virtual void on_read_version_time_out(FsmContextPtr context) {}
    virtual void on_read_head_and_body_time_out(FsmContextPtr context) {}

    virtual bool on_read_version_impl(shared_char_array version, 
        uint32_t len, FsmContextPtr context) = 0;
    
    virtual bool on_read_head_impl(shared_char_array head, 
        uint32_t len, FsmContextPtr context) = 0;

    virtual bool on_read_body_impl(shared_char_array body, 
        uint32_t len, FsmContextPtr context) = 0;
  
    virtual ModState get_succeed_state(FsmContextPtr context) {
      return Mod_Succeed;
    }
    bool cancel_time_out_timer(FsmContextPtr context);
  private:
    void on_read_version(FsmContextPtr context,
        shared_char_array head, 
        uint32_t len, 
        const boost::system::error_code& ec);

    void on_read_head(FsmContextPtr context,
        shared_char_array head, 
        uint32_t len, 
        const boost::system::error_code& ec);

    void on_read_body(FsmContextPtr context,
        shared_char_array body, 
        uint32_t len, 
        const boost::system::error_code& ec);

    void on_read_time_out(uint32_t flag, FsmContextPtr context, 
        const boost::system::error_code & ec);
};


}


#endif
