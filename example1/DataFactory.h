#ifndef __DATA_FACTORY_H__
#define __DATA_FACTORY_H__


#include "Data.h"
#include "../framework/data.h"

class DataFactory : public IFsmDataFactory {
  virtual FsmDataPtr create_data() {
    return FsmDataptr(new FsmData());
  }
};


#endif
