#include "simple_module.h"

namespace zerver {

ModState SimpleModule::run(FsmContextPtr context) {
  return run_impl(context);
}

}
