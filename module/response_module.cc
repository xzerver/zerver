#include "response_module.h"
#include "../framework/fsm.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace zerver {

ModState ResponseModule::run(IFsmData* data) {
  tcp::socket& socket = data->fsm()->socket();

  char* resp = NULL;
  uint32_t resp_len;
  get_resp(&resp, &resp_len);
  boost::asio::async_write(socket, 
      boost::asio::buffer(resp, resp_len),
      boost::bind(&ResponseModule::on_write_resp, this, data,
        boost::asio::placeholders::error));

  uint32_t ms = get_write_resp_time_out_ms();
  timer_.expires_from_now(boost::posix_time::milliseconds(ms));
  timer_.async_wait(boost::bind(&ResponseModule::on_write_time_out, this, data));

  return Mod_async;
}

void ResponseModule::on_write_resp(IFsmData* data, 
    const boost::system::error_code& error) {
  if (!error) {
    data->fsm()->resume(this, Mod_Succeed, data);
  } else {
    on_write_resp_failed();
    data->fsm()->exit();
    data->fsm()->resume(this, Mod_Failed, data);
  }
}

void ResponseModule::on_write_time_out(IFsmData* data) {
  on_write_resp_time_out();

  data->fsm()->exit();
  data->fsm()->resume(this, Mod_Timeout, data);
}
}
