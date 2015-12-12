#ifndef __EXAMPLE_DATA_H__
#define __EXAMPLE_DATA_H__

#include "FsmDataDefines.h"

class Data {
  public:
#include "KaraModuleDataDecl.h"
};


class FsmData : public IFsmData{
  public:
    FsmData(Fsm* fsm) : IFsmData(fsm) {
    }

    DEFINE_MODULE_DATA(KaraModuleData, kara_mod_data)

  private:
    Data* data_;
};


#endif
