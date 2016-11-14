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
#include "request_module.h"
#include "framework/fsm.h"
#include "boost/date_time/posix_time/posix_time.hpp"

DEFINE_int32(wait_ads_request_time_out, 600, "wait read request time from ads server.");
DEFINE_int32(read_ads_body_time_out_ms, 10, "wait read body time from ads server.");

using boost::asio::ip::tcp;

namespace zerver {

ModState RequestModule::run(FsmContextPtr context, ModState last_mod_state) {
  tcp::socket& socket = context->conn()->socket();

  uint32_t version_len = get_version_len(context);
  shared_string_array shared_ver = get_shared_string_array(version_len);
  char* version = shared_ver.get(); 
  boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  uint32_t ms = get_read_version_time_out_ms();
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&RequestModule::on_read_time_out, this, 1, context, boost::asio::placeholders::error));

  boost::asio::async_read(socket,
      boost::asio::buffer(version, version_len),
      boost::bind(&RequestModule::on_read_version, this, context,
        shared_ver, version_len,
        boost::asio::placeholders::error));
  return Mod_async;
}

bool RequestModule::cancel_time_out_timer(FsmContextPtr context) {
  boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  boost::system::error_code err;
  timer.cancel(err);
  if (err) {
    P_LOG(ERROR) << "ERROR: Error when cancel the time out timer: err message:" << err.message();
    return false;
  }

  return true;
}

void RequestModule::on_read_version(
    FsmContextPtr context,
    shared_string_array version, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  if (ec || !on_read_version_impl(version, len, context)) {
    if (ec == boost::asio::error::operation_aborted) {
      context->exit();
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
    //  P_LOG(ERROR) << "read ad req ver failed.";
      on_read_version_failed(context);
      context->exit();
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {
    if (!cancel_time_out_timer(context)) {
      return;
    }
    uint32_t head_len = get_head_len(context);
    tcp::socket& socket = context->conn()->socket();
    shared_string_array shared_head = get_shared_string_array(head_len);
    char* head = shared_head.get();
    uint32_t ms = get_read_head_and_body_time_out_ms();
    boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
    timer.expires_from_now(boost::posix_time::milliseconds(ms));
    timer.async_wait(boost::bind(&RequestModule::on_read_time_out, this, 2, context, boost::asio::placeholders::error));

    boost::asio::async_read(socket, 
        boost::asio::buffer(head, head_len),
        boost::bind(&RequestModule::on_read_head, this, context,
          shared_head, head_len,
          boost::asio::placeholders::error));
  }
}

void RequestModule::on_read_head(FsmContextPtr context,
    shared_string_array head, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  if (ec || !on_read_head_impl(head, len, context)) {
    if (ec == boost::asio::error::operation_aborted) {
      context->exit();
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(ERROR) << "read ad req head failed." << ec.message();
      on_read_head_failed(context);
      context->exit();
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {

    uint32_t body_len = get_body_len(context);

    tcp::socket& socket = context->conn()->socket();
    shared_string_array shared_body =get_shared_string_array(body_len);
    char* body = shared_body.get(); 
    boost::asio::async_read(socket, 
        boost::asio::buffer(body, body_len),
        boost::bind(&RequestModule::on_read_body, this, context,
          shared_body, body_len,
          boost::asio::placeholders::error));
  }
}


void RequestModule::on_read_body(FsmContextPtr context,
    shared_string_array body, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  if (ec || !on_read_body_impl(body, len, context)) {
    if (ec == boost::asio::error::operation_aborted) {
      context->exit();
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(ERROR) << "read ad req body failed.";
      on_read_body_failed(context);
      context->exit();
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {
    if (!cancel_time_out_timer(context)) {
      return;
    }

    ModState state = get_succeed_state(context);
    context->fsm()->resume(this, state, context);
  }
}

void RequestModule::on_read_time_out(uint32_t flag, FsmContextPtr context, 
    const boost::system::error_code & ec) {
  if (ec) {
    // the timer be canceled
    if (ec != boost::asio::error::operation_aborted) {
      P_LOG(ERROR) << "error occured:" << ec.message();
    }
    return;
  }
  if (1 == flag) {
    P_LOG(WARN) << "read ad request version time out: " << 
      get_read_version_time_out_ms() << "ms.";
    on_read_version_time_out(context);
  } else {
    P_LOG(WARN) << "read ad request head or body time out: " << 
      get_read_head_and_body_time_out_ms() << "ms.";
    on_read_head_and_body_time_out(context);
  }

  context->conn()->socket().cancel();
  //context->exit();
}

uint32_t RequestModule::get_read_version_time_out_ms()
{
  return 1000 *FLAGS_wait_ads_request_time_out;
}

uint32_t RequestModule::get_read_head_and_body_time_out_ms()
{
  return FLAGS_read_ads_body_time_out_ms;
}

}
