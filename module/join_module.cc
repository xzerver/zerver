#include "join_module.h"

namespace zerver {

ModState JoinModule::run(FsmContextPtr context, ModState last_mod_state) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());

  data->lock(); 
  if (last_mod_state != Mod_Succeed) {
    data->failed_join();
  }

  uint16_t module_joined = data->module_joined();
  data->unlock();
  if (module_joined == get_active_join_num(context)) {
//    ModState state = get_state(context);
    ModState state = (data->get_failed_join() ? Mod_Failed : Mod_Succeed);
    boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
    timer.expires_from_now(boost::posix_time::milliseconds(0));
    timer.async_wait(boost::bind(&JoinModule::on_joined, this, context, state, boost::asio::placeholders::error));
}

  return Mod_async;
}

void JoinModule::on_joined(FsmContextPtr context, 
    ModState state, 
    const boost::system::error_code & ec) {
  context->fsm()->resume(this, state, context);
}
}
