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
#include "response_module.h"
#include "../framework/fsm.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace zerver {

ModState ResponseModule::run(FsmContextPtr context) {
  tcp::socket& socket = context->conn()->socket();

  char* resp = NULL;
  uint32_t resp_len;
  get_resp(&resp, &resp_len, context);
  boost::asio::async_write(socket, 
      boost::asio::buffer(resp, resp_len),
      boost::bind(&ResponseModule::on_write_resp, this, context,
        boost::asio::placeholders::error));

  uint32_t ms = get_write_resp_time_out_ms();
  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&ResponseModule::on_write_time_out, this, context));

  return Mod_async;
}

void ResponseModule::on_write_resp(FsmContextPtr context, 
    const boost::system::error_code& error) {
  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.cancel();

  if (!error) {
    on_write_resp_succeed(context);
    context->fsm()->resume(this, Mod_Succeed, context);
  } else {
    on_write_resp_failed(context);
    context->fsm()->exit();
    context->fsm()->resume(this, Mod_Failed, context);
  }
}

void ResponseModule::on_write_time_out(FsmContextPtr context) {
  on_write_resp_time_out(context);

  tcp::socket& socket = context->conn()->socket();
  socket.cancel();

  context->fsm()->exit();
  context->fsm()->resume(this, Mod_Timeout, context);
}
}
