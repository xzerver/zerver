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
#include <sys/time.h>

int my_rand(void)
{
  static  boost::thread_specific_ptr<unsigned int> seed;
  if (NULL == seed.get()) {
     struct  timeval tv;
     gettimeofday(&tv, NULL);
     seed.reset(new unsigned int(tv.tv_sec * tv.tv_usec) );
  }
  return rand_r(seed.get());
}

namespace zerver {

IFsmDataFactory* FsmContext::fsm_data_factory_ = NULL;

void FsmContext::register_fsm_data_factory(IFsmDataFactory* factory)
{
  fsm_data_factory_ = factory;
}

FsmContext::FsmContext(TcpConnectionPtr conn, Fsm* fsm, boost::asio::io_service& io_service) 
: exit_(false), fsm_(fsm), conn_(conn), io_service_(io_service)
{
  uuid_ = 0;
}

void FsmContext::init() {
  data_ = fsm_data_factory_->create_data();

  const ModuleMap& mod_map = fsm_->get_module_map();
  for (ModuleMap::const_iterator it = mod_map.begin();
      it != mod_map.end(); ++it) {
    ModuleDataPtr mod_data = it->second->create_data(this);
    if (mod_data.get())
      module_data_map_[it->first] = mod_data;
  }
}

bool FsmContext::recollectable() {
  std::map<std::string, ModuleDataPtr>::const_iterator it = module_data_map_.begin();
  for (; it != module_data_map_.end(); it++) {
    if (!it->second->recollectable()) {
      return false;
    }
  }
  return true;
}

void FsmContext::reset() {
  exit_ = false;
  data_->reset();
  
  uuid_ = my_rand();
  for (std::map<std::string, ModuleDataPtr>::const_iterator it = module_data_map_.begin();
      it != module_data_map_.end(); ++it) {
    it->second->reset();
  }
}

}

