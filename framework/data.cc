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
#include "data.h"


namespace zerver {

FsmContext::FsmContext(TcpConnectionPtr conn) : conn_(conn) {
  fsm_ = new Fsm();
  data_ = fsm_data_factory_->create_data();

  const ModuleMap& mod_map = Fsm::get_module_map();
  for (ModuleMap::const_iterator it = mod_map.begin();
      it != mod_map.end(); ++it) {
    ModuleDataPtr mod_data = it->second->create_data();
    module_data_map_[it->first] = mod_data;
  }
}

FsmContext::~FsmContext() {
  delete fsm_;
}

void FsmContext::reset() {
  data_->reset();
}

void FsmContext::lock() {
}

void FsmContext::unlock() {
}

}
