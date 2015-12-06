#ifndef __ZERVER_SIMPLE_MODULE_H__
#define __ZERVER_SIMPLE_MODULE_H__

#include "../framework/module.h"

namespace zerver {

class SimpleModule : public Module {
  public:
    SimpleModule(const std::string& name) : Module(name) {
    }
    bool is_async() { return false; }

    ModState run(FsmContextPtr context);

  protected:
    virtual ModState run_impl(FsmContextPtr context) = 0;
};

}

#endif
