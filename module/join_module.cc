#include "join_module.h"

namespace zerver {

ModState JoinModule::run(FsmContextPtr context) {
  ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());
 
  data->module_joined(); 

  if (data->get_module_joined_num() == in_module_num_) {
    ModState state = get_state(context);
    context->fsm()->resume(this, state, context);
  }

  return Mod_async;
}

void JoinModule::on_link_in(ModState state) {
  in_module_num_ ++;
}

}
