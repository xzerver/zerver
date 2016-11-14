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

Fsm::~Fsm() {
  ModuleMap::iterator it = mod_map_.begin();
  for (; it != mod_map_.end(); it++) {
    delete it->second;
  }

  mod_map_.clear();
}

void Fsm::add_module(Module* mod) {
  if (mod_map_.find(mod->name()) != mod_map_.end()) {
    F_LOG(ERROR) << "add module " << mod->name() << " failed, there's one with the same name!";
    return;
  }
  mod_map_[mod->name()] = mod;
}

void Fsm::link_module(Module* from, Module* to, ModState state) {
  ModuleTransitionMap::iterator it = mod_trans_map_.find(from->name());
  if (it != mod_trans_map_.end()) {
    it->second.insert(std::make_pair(state, to->name()));
  } else {
    std::map<ModState, std::string> trans_map;
    trans_map.insert(std::make_pair(state, to->name()));
    mod_trans_map_.insert(std::make_pair(from->name(), trans_map));
  }
}

void Fsm::handle_request(FsmContextPtr context) {
  context->reset();
  _run(start_mod_, context, Mod_start);
}

void Fsm::_run(Module* mod, FsmContextPtr context, ModState last_mod_state) {
  //P_DLOG() << "mod:" << mod->name() << " run.";
  ModState state = mod->run(context, last_mod_state);
  if (state != Mod_async) {
    if (mod == end_mod_) {
      return _end_request(context);
    }
    Module* next_mod = _get_next_module(context, mod, state);
    if (!next_mod) {
      context->exit();
      _end_request(context);
      return;
    }
    _run(next_mod, context, state);
  }
}

void Fsm::resume(Module* last_mod, ModState state, FsmContextPtr context) {
  if (last_mod == end_mod_) {
    return _end_request(context);
  }
  Module* next_mod = _get_next_module(context, last_mod, state);
  if (!next_mod) {
    context->exit();
    _end_request(context);
    return;
  }
  _run(next_mod, context, state);
}

Module* Fsm::_get_next_module(FsmContextPtr context, Module* mod, ModState state) {
  ModuleTransitionMap::const_iterator it = mod_trans_map_.find(mod->name());
  if (it == mod_trans_map_.end()) {
    // error here
    P_LOG(ERROR) << "can't find module " << mod->name();
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
  P_LOG(ERROR) << "can't find next module of " << mod->name() << " by state trans from " << state;
  return NULL;
}

void Fsm::_end_request(FsmContextPtr context) {
  if (!context->is_exit()) {
    if (true || !context->recollectable()) {
      context = FsmContextPtr(new FsmContext(context->conn(), this, context->io_service()));
      context->init();
    }
    handle_request(context);
  }
}

}
