#include "request_module.h"
#include "../framework/fsm.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::asio::ip::tcp;

namespace zerver {

ModState RequestModule::run(IFsmData* data) {
  tcp::socket& socket = data->fsm()->socket();

  uint32_t head_len = get_head_len();
  char* head = new char[head_len];
  boost::asio::async_read(socket,
      boost::asio::buffer(head, head_len),
      boost::bind(&RequestModule::on_read_head, this, data,
        boost::shared_array<char>(head), head_len,
        boost::asio::placeholders::error));

  uint32_t ms = get_read_head_time_out_ms();
  timer_.expires_from_now(boost::posix_time::milliseconds(ms));
  timer_.async_wait(boost::bind(&RequestModule::on_read_time_out, this, 1, data));
  return Mod_async;
}

void RequestModule::on_read_head(IFsmData* data,
    boost::shared_array<char> head, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  timer_.cancel();

  if (ec) {
    on_read_head_failed();
    data->fsm()->exit();
    data->fsm()->resume(this, Mod_Failed, data);
    return;
  }

  uint32_t body_len = get_body_len(head.get(), len);
  // check head failed
  if (body_len == -1) {
    on_read_head_failed();
    data->fsm()->exit();
    data->fsm()->resume(this, Mod_Failed, data);
    return;
  }

  tcp::socket& socket = data->fsm()->socket();

  char* body = new char[body_len];
  boost::asio::async_read(socket, 
      boost::asio::buffer(body, body_len),
      boost::bind(&RequestModule::on_read_body, this, data,
        boost::shared_array<char>(body), body_len,
        boost::asio::placeholders::error));

  uint32_t ms = get_read_body_time_out_ms();
  timer_.expires_from_now(boost::posix_time::milliseconds(ms));
  timer_.async_wait(boost::bind(&RequestModule::on_read_time_out, this, 2, data));
}


void RequestModule::on_read_body(IFsmData* data,
    boost::shared_array<char> body, 
    uint32_t len, 
    const boost::system::error_code& ec) {
  timer_.cancel();

  if (ec) {
    on_read_body_failed();
    data->fsm()->exit();
    data->fsm()->resume(this, Mod_Failed, data);
    return;
  }

  bool ret = on_read_body_impl(body, len);
  ModState state = (ret ? Mod_Succeed : Mod_Failed);
  if (!ret) {
    on_read_body_failed();
    data->fsm()->exit();
    data->fsm()->resume(this, Mod_Failed, data);
    return;
  }
  data->fsm()->resume(this, Mod_Succeed, data);
}

void RequestModule::on_read_time_out(uint32_t flag, IFsmData* data) {
  if (1 == flag) {
    on_read_head_time_out();
  } else {
    on_read_body_time_out();
  }

  data->fsm()->exit();
  data->fsm()->resume(this, Mod_Timeout, data);
}


}
