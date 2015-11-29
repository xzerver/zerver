#ifndef __ZERVER_FSM_H__
#define __ZERVER_FSM_H__


#include "data.h"
#include "module.h"

using boost::asio::ip::tcp;

namespace zerver {

class Module;

typedef std::map<Module::Name, Module*> ModuleMap;
typedef std::map<Module::Name, std::map<ModState, Module::Name> > ModuleTransitionMap;
class Fsm {
  public:
    Fsm(tcp::socket& socket) : socket_(socket) {
    }

    typedef boost::shared_ptr<Fsm> pointer;
    static pointer create_fsm(tcp::socket& socket) {
      return boost::shared_ptr<Fsm>(new Fsm(socket));
    }

    void add_module(Module* mod);
    void set_start_module(Module* mod);
    void set_end_module(Module* mod);
    void link_module(Module* from, Module* to, ModState state);
    void register_fsm_data_factory(IFsmDataFactory* factory);

    void handle_request();
    void resume(Module* last_mod, ModState state, IFsmData* data);
    tcp::socket& socket() { return socket_; }

  private:
    void _run(Module* mod, IFsmData* data);
    IFsmData* _get_data();
    Module* _get_next_module(Module* mod, ModState state);
    void _end_request(IFsmData* data);

    tcp::socket& socket_;

    ModuleTransitionMap mod_trans_map_;
    ModuleMap mod_map_;
    Module* start_mod_;
    Module* end_mod_;
    IFsmDataFactory* fsm_data_factory_;
    bool exit_;
};
}

#endif
