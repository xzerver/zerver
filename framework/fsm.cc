#include "fsm.h"

namespace zerver {

void Fsm::handle_request(FsmContextPtr context) {
  context->reset();
  _run(start_mod_, context);
}

void Fsm::_run(Module* mod, FsmContextPtr context) {
  if (mod->is_async()) {
    mod->run(context);
  } else {
    ModState state = mod->run(context);
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
