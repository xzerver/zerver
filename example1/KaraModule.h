#include "../module/simple_module.h"
#include "ModuleDefines.h"
#include "KaraModuleData.h"

namespace example {



BEGIN_CLASS(KaraModule, SimpleModule, KaraModuleData)

public:
  KaraModule(const std::string& name) : SimpleModule(name) {
  }

protected:
  virtual ModState run_impl(FsmContextPtr context);


ENDCLASS(KaraModule)


}
