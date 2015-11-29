#include "fsm.h"

namespace zerver {

void Fsm::handle_request() {
  IFsmData* data = _get_data();
  _run(start_mod_, data);
}

void Fsm::_run(Module* mod, IFsmData* data) {
  if (mod->is_async()) {
    mod->run(data);
  } else {
    ModState state = mod->run(data);
    if (mod == end_mod_) {
      return _end_request(data);
    }
    Module* next_mod = _get_next_module(mod, state);
    _run(next_mod, data);
  }
}

void Fsm::resume(Module* last_mod, ModState state, IFsmData* data) {
  if (last_mod == end_mod_) {
    return _end_request(data);
  }
  Module* next_mod = _get_next_module(last_mod, state);
  _run(next_mod, data);
}

Module* Fsm::_get_next_module(Module* mod, ModState state) {
  ModuleTransitionMap::const_iterator it = mod_trans_map_.find(mod->name());
  if (it == mod_trans_map_.end()) {
    // error here
    return NULL;
  }

  const std::map<ModState, Module::Name>& state_mod_map = it->second;
  std::map<ModState, Module::Name>::const_iterator iter = state_mod_map.find(state);
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

void Fsm::_end_request(IFsmData* data) {
  if (!exit_)
    _run(start_mod_, data);
  else
    fsm_data_factory_->delete_data(data);
}

IFsmData* Fsm::_get_data() {
  IFsmData* data = fsm_data_factory_->create_data(this);
}

}
