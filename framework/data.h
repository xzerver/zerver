#ifndef __ZERVER_FSM_DATA_H__
#define __ZERVER_FSM_DATA_H__

#include <sys/types.h>
#include <vector>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "types_def.h"

namespace zerver {

class Fsm;
class IFsmData {
  public:
    IFsmData(Fsm* fsm) : fsm_(fsm) {
    }

    Fsm* fsm() { return fsm_; }
  private:
    Fsm* fsm_;
};

class IFsmDataFactory {
  public:
    virtual IFsmData* create_data(Fsm* fsm) = 0;
    virtual IFsmData* delete_data(IFsmData* data) = 0;
};

}

#endif
