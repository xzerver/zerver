#ifndef __ZERVER_FSM_H__
#define __ZERVER_FSM_H__


#include "data.h"
#include "module.h"

using boost::asio::ip::tcp;

namespace zerver {

class Fsm {
  public:
    Fsm() : exit_(false) {
    }

    static void add_module(Module* mod);
    static void set_start_module(Module* mod);
    static void set_end_module(Module* mod);
    static void link_module(Module* from, Module* to, ModState state);
    static const ModuleMap& get_module_map();

    void handle_request(FsmContextPtr context);
    void resume(Module* last_mod, ModState state, FsmContextPtr context);
    void exit();

  private:
    void _run(Module* mod, FsmContextPtr context);
    Module* _get_next_module(Module* mod, ModState state);
    void _end_request(FsmContextPtr context);

    static ModuleTransitionMap mod_trans_map_;
    static ModuleMap mod_map_;
    static Module* start_mod_;
    static Module* end_mod_;
    bool exit_;
};
}

#endif
