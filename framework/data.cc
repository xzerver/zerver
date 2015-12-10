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

void FsmContext::reset() {
}

FsmContext::~FsmContext() {
  delete fsm_;
}

}
