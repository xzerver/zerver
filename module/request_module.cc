#include "request_module.h"
#include "../framework/fsm.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;

namespace zerver {

ModState RequestModule::run(FsmContextPtr context) {
  tcp::socket& socket = context->conn()->socket();

  uint32_t head_len = get_head_len();
  char* head = new char[head_len];
  boost::asio::async_read(socket,
      boost::asio::buffer(head, head_len),
      boost::bind(&RequestModule::on_read_head, this, context,
        boost::shared_array<char>(head), head_len,
        boost::asio::placeholders::error));

  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  uint32_t ms = get_read_head_time_out_ms();
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&RequestModule::on_read_time_out, this, 1, context));
  return Mod_async;
}

void RequestModule::on_read_head(FsmContextPtr context,
    boost::shared_array<char> head, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.cancel();

  if (ec) {
    on_read_head_failed(context);
    context->fsm()->exit();
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  }

  uint32_t body_len = get_body_len(head.get(), len, context);
  // check head failed
  if (body_len == -1) {
    on_read_head_failed(context);
    context->fsm()->exit();
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  }

  tcp::socket& socket = context->conn()->socket();

  char* body = new char[body_len];
  boost::asio::async_read(socket, 
      boost::asio::buffer(body, body_len),
      boost::bind(&RequestModule::on_read_body, this, context,
        boost::shared_array<char>(body), body_len,
        boost::asio::placeholders::error));

  uint32_t ms = get_read_body_time_out_ms();
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&RequestModule::on_read_time_out, this, 2, context));
}


void RequestModule::on_read_body(FsmContextPtr context,
    boost::shared_array<char> body, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.cancel();

  if (ec) {
    on_read_body_failed(context);
    context->fsm()->exit();
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  }

  bool ret = on_read_body_impl(body, len, context);
  if (!ret) {
    context->fsm()->exit();
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  }
  context->fsm()->resume(this, Mod_Succeed, context);
}

void RequestModule::on_read_time_out(uint32_t flag, FsmContextPtr context) {
  if (1 == flag) {
    on_read_head_time_out(context);
  } else {
    on_read_body_time_out(context);
  }

  context->conn()->socket().cancel();
  context->fsm()->exit();
  context->fsm()->resume(this, Mod_Timeout, context);
}


}
