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
#include "boost/date_time/posix_time/posix_time.hpp"

namespace zerver {

ModState ResponseModule::run(FsmContextPtr context, ModState last_mod_state) {
  tcp::socket& socket = context->conn()->socket();

  uint32_t resp_len;
  shared_char_array resp = get_resp(&resp_len, context);
  if (!resp.get()) {
    return Mod_Failed;
  }
  uint32_t ms = get_write_resp_time_out_ms();
  //P_DLOG() << "resp_len:" << resp_len;
  boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&ResponseModule::on_write_time_out, this, context, boost::asio::placeholders::error));
  boost::asio::async_write(socket, 
      boost::asio::buffer(resp.get(), resp_len),
      boost::bind(&ResponseModule::on_write_resp, this, context,
        boost::asio::placeholders::error));

  return Mod_async;
}

void ResponseModule::on_write_resp(FsmContextPtr context, 
    const boost::system::error_code& error) {

  if (!error) {
    if (!cancel_time_out_timer(context)) {
      return;
    }
    on_write_resp_succeed(context);
    context->fsm()->resume(this, Mod_Succeed, context);
  } else {
    if (error == boost::asio::error::operation_aborted) {
      context->exit();
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(WARN) << name() << ":on_write_resp_failed";
      on_write_resp_failed(context);
      context->exit();
      context->fsm()->resume(this, Mod_Failed, context);
    }
  }
}

bool ResponseModule::cancel_time_out_timer(FsmContextPtr context) {
  boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  boost::system::error_code err;
  timer.cancel(err);
  if (err) {
    P_LOG(ERROR) << "ERROR: Error when cancel the time out timer: err message:" << err.message();
    return false;
  }
  return true;
}

void ResponseModule::on_write_time_out(FsmContextPtr context, const boost::system::error_code & ec) {
  if (ec) {
    // the timer be canceled
    if (ec != boost::asio::error::operation_aborted) {
      P_LOG(ERROR) << "error occured:" << ec.message();
    }
    return;
  }
  P_LOG(WARN) << name() << ":on_write_resp_time_out, time:" << get_write_resp_time_out_ms();
  on_write_resp_time_out(context);

  tcp::socket& socket = context->conn()->socket();
  socket.cancel();

//  context->exit();
//  context->fsm()->resume(this, Mod_Timeout, context);
}
}
