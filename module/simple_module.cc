#include "simple_module.h"

namespace zerver {

ModState SimpleModule::run(IFsmData* data) {
  return run_impl(data);
}

}
