#ifndef __KARA_MODULE_H__
#define __KARA_MODULE_H__

#include "../module/simple_module.h"
#include "../macro_define/ModuleDefines.h"
#include "KaraModuleData.h"
#include "FsmData.h"

using namespace zerver;

BEGIN_CLASS(KaraModule, SimpleModule, KaraModuleData)

public:
  KaraModule(const std::string& name) : SimpleModule(name) {
  }

protected:
  virtual ModState run_impl(FsmContextPtr context);

ENDCLASS()

#endif
