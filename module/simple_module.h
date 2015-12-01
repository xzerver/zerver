#ifndef __ZERVER_SIMPLE_MODULE_H__
#define __ZERVER_SIMPLE_MODULE_H__

#include "../framework/module.h"

namespace zerver {

class SimpleModule : public Module {
  public:
    bool is_async() { return false; }

    ModState run(IFsmData* data);

  protected:
    virtual ModState run_impl(IFsmData* data) = 0;
};

}

#endif
