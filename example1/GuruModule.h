#ifndef __GURU_MODULE_H__
#define __GURU_MODULE_H__

#include "../module/simple_module.h"
#include "../macro_define/ModuleDefines.h"
#include "GuruModuleData.h"
#include "FsmData.h"

using namespace zerver;

BEGIN_CLASS(GuruModule, SimpleModule, GuruModuleData)

public:
  GuruModule(const std::string& name) : SimpleModule(name) {
  }

protected:
  virtual ModState run_impl(FsmContextPtr context);

ENDCLASS()

#endif
