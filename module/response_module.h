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
#ifndef __ZERVER_RESPONSE_MODULE_H__
#define __ZERVER_RESPONSE_MODULE_H__

#include "framework/module.h"

namespace zerver {
class ResponseModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        ModuleData(FsmContext* context) : timer_(context->io_service()) {
        }
        boost::asio::deadline_timer timer_;
    };

    ResponseModule(const std::string& name) : Module(name) {
    }
    virtual ModState run(FsmContextPtr context, ModState last_mod_state);

    virtual ModuleDataPtr create_data(FsmContext* context) { 
      return ModuleDataPtr(new ModuleData(context)); 
    }
  protected:
    bool cancel_time_out_timer(FsmContextPtr context);
    virtual shared_string_array get_resp(uint32_t* resp_len, FsmContextPtr context) = 0;
    virtual void on_write_resp_failed(FsmContextPtr context) {}
    virtual void on_write_resp_time_out(FsmContextPtr context) {}
    virtual void on_write_resp_succeed(FsmContextPtr context) {}
    virtual uint32_t get_write_resp_time_out_ms() { return 30; }

  private:
    void on_write_resp(FsmContextPtr context, 
        const boost::system::error_code& error);
    void on_write_time_out(FsmContextPtr context, const boost::system::error_code & ec);
};

}

#endif
