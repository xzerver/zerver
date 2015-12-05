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
#include "module.h"
#include "fsm.h"
#include "tcp_connection.h"


namespace zerver {

class IFsmData {
  public:
    IFsmData()  {
    }

};


class IFsmDataFactory {
  public:
    virtual FsmDataPtr create_data() = 0;
};

class FsmContext {
  public:
    FsmContext(TcpConnectionPtr conn);
    virtual ~FsmContext();

    Fsm* fsm() { return fsm_; }
    void reset();
    FsmDataPtr data() { return data_; }
    TcpConnectionPtr conn() { return conn_; }
    ModuleDataPtr get_module_data(const std::string& name) { return module_data_map_[name]; }
    static void register_fsm_data_factory(IFsmDataFactory* factory);
  private:
    Fsm* fsm_;
    TcpConnectionPtr conn_;

    FsmDataPtr data_;
    std::map<std::string, ModuleDataPtr> module_data_map_;
    static IFsmDataFactory* fsm_data_factory_;
};

}

#endif
