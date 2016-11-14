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
#ifndef __ZERVER_FORK_MODULE_H__
#define __ZERVER_FORK_MODULE_H__

#include "../framework/module.h"

const ModState Mod_Fork0 = 840;
const ModState Mod_Fork1 = 841;
const ModState Mod_Fork2 = 842;
const ModState Mod_Fork3 = 843;
const ModState Mod_Fork4 = 844;
const ModState Mod_Fork5 = 845;
const ModState Mod_Fork6 = 846;
const ModState Mod_Fork7 = 847;
const ModState Mod_Fork8 = 848;
const ModState Mod_Fork9 = 849;
const ModState Mod_Fork10 = 850;
const ModState Mod_Fork11 = 851;
const ModState Mod_Fork12 = 852;
const ModState Mod_Fork13 = 853;
const ModState Mod_Fork14 = 854;
const ModState Mod_Fork15 = 855;

namespace zerver {

class ForkModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        ModuleData(FsmContext* context, uint16_t fork_num) : fork_num_(fork_num) {
          timer_ = new boost::asio::deadline_timer*[fork_num];
          for (int i=0; i<fork_num; i++) {
            timer_[i] = new boost::asio::deadline_timer(context->io_service());
          }
        }

        ~ModuleData() {
          for (int i=0; i<fork_num_; i++) {
            delete timer_[i];
          }
          delete [] timer_;
        }

        void reset() {
        }

        uint16_t fork_num_;
        boost::asio::deadline_timer** timer_;
    };
    ForkModule(const std::string& name, uint16_t fork_num) : Module(name), fork_num_(fork_num) {
    }

    virtual ModuleDataPtr create_data(FsmContext* context) { 
      return ModuleDataPtr(new ModuleData(context, fork_num_)); 
    }

    ModState run(FsmContextPtr context, ModState last_mod_state);

    virtual uint16_t get_active_fork_num(FsmContextPtr context) { return fork_num_; }
  protected:
    uint16_t fork_num_;

    void on_forked(FsmContextPtr context, ModState fork_index, 
        const boost::system::error_code & ec);
};

}

#endif
