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

namespace zerver {

class JoinModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        void reset() {
          module_joined_num_ = 0;
        }
        void module_joined() {
          module_joined_num_ ++;
        }
        uint16_t get_module_joined_num() {
          return module_joined_num_;
        }
      private:
        uint16_t module_joined_num_;
    };

    JoinModule(const std::string& name) : Module(name) {
    }
    bool is_async() { return true; }

    virtual ModuleDataPtr create_data() { 
      return ModuleDataPtr(new ModuleData); 
    }
    ModState run(FsmContextPtr context);
    virtual void on_link_in(ModState state);

    // It's not easy to state the mod_state of a JoinModule, so give it to the application who use this framework
    virtual ModState get_state(FsmContextPtr context) {
      return Mod_Succeed;
    }
  protected:
    uint16_t in_module_num_;
};

}

#endif
