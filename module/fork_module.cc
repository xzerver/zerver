#include "fork_module.h"

namespace zerver {

ModState ForkModule::run(FsmContextPtr context, ModState last_mod_state) {
  ModuleDataPtr model_data;
  for (size_t i = 0; i < get_active_fork_num(context); i++) {
    model_data = context->get_module_data(name());
    ModuleData *ptr = dynamic_cast<ModuleData*>(model_data.get());
    boost::asio::deadline_timer* timer = ptr->timer_[i];
    timer->expires_from_now(boost::posix_time::milliseconds(0));
    timer->async_wait(boost::bind(&ForkModule::on_forked, this, context, Mod_Fork0 + i, boost::asio::placeholders::error));
  }
  return Mod_async;
}


void ForkModule::on_forked(FsmContextPtr context, 
    ModState fork_index, 
    const boost::system::error_code & ec) {
  context->fsm()->resume(this, fork_index, context);
}
}


