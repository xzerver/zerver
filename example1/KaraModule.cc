#include "KaraModule.h"

ModState KaraModule::run_impl(FsmContextPtr context) {
  getKaraModuleData(context)->set_id(10);
  const std::string& name = getKaraModuleData(context)->get_const_name();
  CustomData1& data1 = getKaraModuleData(context)->get_mutable_data1();
  return Mod_Succeed;
}
