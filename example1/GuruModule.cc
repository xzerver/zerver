#include "GuruModule.h"

ModState GuruModule::run_impl(FsmContextPtr context) {
  int id = getGuruModuleData(context)->get_id();
  getGuruModuleData(context)->set_school(40);
  std::string& name = getGuruModuleData(context)->get_mutable_name();
  name = "abc";
  const CustomData1& data1 = getGuruModuleData(context)->get_const_data1();
  return Mod_Succeed;
}
