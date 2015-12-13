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
#ifndef __ZERVER_SIMPLE_MODULE_H__
#define __ZERVER_SIMPLE_MODULE_H__

#include "../framework/module.h"

namespace zerver {

class SimpleModule : public Module {
  public:
    SimpleModule(const std::string& name) : Module(name) {
    }
    bool is_async() { return false; }

    ModState run(FsmContextPtr context);

  protected:
    virtual ModState run_impl(FsmContextPtr context) = 0;
};

}

#endif
