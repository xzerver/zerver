#ifndef __DATA_FACTORY_H__
#define __DATA_FACTORY_H__


#include "FsmData.h"
#include "../framework/data.h"

using namespace zerver;

class DataFactory : public IFsmDataFactory {
  virtual FsmDataPtr create_data() {
    return FsmDataPtr(new FsmData());
  }
};


#endif
