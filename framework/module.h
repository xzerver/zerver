#ifndef __ZERVER_MODULE_H__
#define __ZERVER_MODULE_H__

#include <string>
#include "types_def.h"
#include "data.h"

namespace zerver {

const ModState Mod_Succeed = 1;
const ModState Mod_Failed = 2;
const ModState Mod_Timeout = 3;
const ModState Mod_else = 0;
const ModState Mod_async = 4;

class IModuleData {
};


class Module {
  public:
    Module(const std::string& name) : name_(name) {
    }

    virtual ModuleDataPtr create_data() { return ModuleDataPtr(new IModuleData); }

    virtual bool is_async() = 0;
    virtual ModState run(FsmContextPtr context) = 0;

    const std::string& name() const { return name_; }

  protected:
    std::string name_;
};


}

#endif
