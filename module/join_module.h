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
#ifndef __ZERVER_JOIN_MODULE_H__
#define __ZERVER_JOIN_MODULE_H__

#include "../framework/module.h"
#include "boost/atomic.hpp"
#include "boost/thread/mutex.hpp"

namespace zerver {

class JoinModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        ModuleData(FsmContext* context) : timer_(context->io_service()) {
        }
        void lock() {
          lock_.lock();
        }
        void unlock() {
          lock_.unlock();
        }
        void reset() {
          module_joined_num_ = 0;
          failed_join_ = false;
        }
        uint16_t module_joined() {
          return ++module_joined_num_;
        }
        void failed_join() {
          failed_join_ = true;
        }
        bool get_failed_join() {
          return failed_join_;
        }

        boost::asio::deadline_timer timer_;
      private:
        // TODO: this variable should be thread safe
        uint16_t module_joined_num_;
        bool failed_join_;
        boost::mutex lock_;
    };

    JoinModule(const std::string& name, uint16_t join_num) : Module(name), join_num_(join_num) {
    }

    virtual ModuleDataPtr create_data(FsmContext* context) { 
      return ModuleDataPtr(new ModuleData(context)); 
    }
    ModState run(FsmContextPtr context, ModState last_mod_state);

    // It's not easy to state the mod_state of a JoinModule, so give it to the application who use this framework
/*    virtual ModState get_state(FsmContextPtr context) {
      return Mod_Succeed;
    }
    */
    virtual uint16_t get_active_join_num(FsmContextPtr context) { return join_num_; }
  protected:
    void on_joined(FsmContextPtr context, 
        ModState state, 
        const boost::system::error_code & ec);
    uint16_t join_num_;
};

}

#endif
