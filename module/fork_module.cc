#include "fork_module.h"

namespace zerver {

ModState ForkModule::run(FsmContextPtr context) {
  for (size_t i = 0;i < next_module_states_.size(); i++) {
    context->fsm()->resume(this, next_module_states_[i], context);
  }
  return Mod_async;
}

void ForkModule::on_link_out(ModState state) {
  next_module_states_.push_back(state);
}
}


