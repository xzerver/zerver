#include "response_module.h"
#include "../framework/fsm.h"

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
  return Mod_async;
}

void ResponseModule::on_write_resp(IFsmData* data, 
    const boost::system::error_code& error) {
  if (!error) {
    data->fsm()->resume(this, Mod_Succeed, data);
  } else {
    data->fsm()->exit();
    data->fsm()->resume(this, Mod_Failed, data);
  }
}

}
