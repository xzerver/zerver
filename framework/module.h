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
#ifndef __ZERVER_MODULE_H__
#define __ZERVER_MODULE_H__

#include <string>
#include "types_def.h"
#include "data.h"

namespace zerver {

const ModState Mod_Succeed = 1;
const ModState Mod_Failed = 2;
const ModState Mod_Timeout = 3;
const ModState Mod_else = 0;
const ModState Mod_async = 4;
const ModState Mod_start = 5;

class IModuleData {
  public:
    IModuleData()
    {
      //P_LOG(INFO) << "IModuleData New:" << this;
    }

    virtual ~IModuleData() {
      //P_LOG(INFO) << "IModuleData Delete:" << this;
    }

    virtual bool recollectable() {
      return true;
    }

    virtual void reset() {
    }
};


class Module {
  public:
    Module(const std::string& name) : name_(name) {
    }
    virtual ~Module() {}

    // module that have one's own ModuleData should overide this method
    virtual ModuleDataPtr create_data(FsmContext* context) { return ModuleDataPtr(); }

    virtual ModState run(FsmContextPtr context, ModState last_mod_state) = 0;
    virtual void on_link_out(ModState state) {}
    virtual void on_link_in(ModState state) {}

    const std::string& name() const { return name_; }

  protected:
    std::string name_;
};


}

#endif
