// Copyright (C) 2014, 2015 Fanxi Zeng
// Contact: zengfancy@126.com 
// 
// This file is part of xzerver.
// 
// xzerver is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// xzerver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with xzerver. If not, see <http://www.gnu.org/licenses/>.
//
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
