#ifndef __ZERVER_MODULE_H__
#define __ZERVER_MODULE_H__

#include <string>
#include "types_def.h"

namespace zerver {

typedef uint32_t ModState;

const ModState Mod_Succeed = 1;
const ModState Mod_Failed = 2;
const ModState Mod_Timeout = 3;
const ModState Mod_else = 0;
const ModState Mod_async = 4;

class IFsmData;

class Module {
  public:
    typedef std::string Name;
    Module(const Name& name) : name_(name) {
    }

    virtual bool is_async() = 0;
    virtual ModState run(IFsmData* data) = 0;

    const Name& name() const { return name_; }

  protected:
    Name name_;
};

}

#endif
