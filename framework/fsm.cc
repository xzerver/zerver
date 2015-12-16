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
#include "fsm.h"

namespace zerver {

void Fsm::handle_request(FsmContextPtr context) {
  context->reset();
  _run(start_mod_, context);
}

void Fsm::_run(Module* mod, FsmContextPtr context) {
  if (mod->is_async()) {
    // lock the context data because it's a thread-parallel process
    context->lock();
    mod->run(context);
    context->unlock();
  } else {
    // lock the context data because it's a thread-parallel process
    context->lock();
    ModState state = mod->run(context);
    context->unlock();
    if (mod == end_mod_) {
      return _end_request(context);
    }
    Module* next_mod = _get_next_module(mod, state);
    _run(next_mod, context);
  }
}

void Fsm::resume(Module* last_mod, ModState state, FsmContextPtr context) {
  if (last_mod == end_mod_) {
    return _end_request(context);
  }
  Module* next_mod = _get_next_module(last_mod, state);
  _run(next_mod, context);
}

Module* Fsm::_get_next_module(Module* mod, ModState state) {
  ModuleTransitionMap::const_iterator it = mod_trans_map_.find(mod->name());
  if (it == mod_trans_map_.end()) {
    // error here
    return NULL;
  }

  const std::map<ModState, std::string>& state_mod_map = it->second;
  std::map<ModState, std::string>::const_iterator iter = state_mod_map.find(state);
  if (iter != state_mod_map.end()) {
    return mod_map_[iter->second];
  }

  iter = state_mod_map.find(Mod_else);
  if (iter != state_mod_map.end()) {
    return mod_map_[iter->second];
  }

  // error
  return NULL;
}

void Fsm::_end_request(FsmContextPtr context) {
  if (!exit_)
    handle_request(context);
}

void Fsm::exit() {
  exit_ = true;
}

}
