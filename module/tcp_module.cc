#include "tcp_module.h"

namespace zerver {


ModState TcpModule::run(FsmContextPtr context) {
  uint32_t client_type_id = get_client_type_id(context);
  ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());
  data->client_ = client_pool_.get_client(client_type_id);

  uint32_t ms = get_time_out_ms();
  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&TcpModule::on_handle_time_out, this, context));

  if (!data->client_) {
    std::string ip;
    unsigned short port;
    get_server_ip_port(ip, port, context);
    boost::asio::ip::tcp::endpoint endpoint(
            boost::asio::ip::address::from_string(ip), port);

    data->client_ = TcpClientPtr(new TcpClient(context->conn()->io_service(), client_type_id));

    data->client_->socket().async_connect(endpoint, 
        boost::bind(&TcpModule::on_connect, this, context,
          boost::asio::placeholders::error));
    return Mod_async;
  }

  return send_request(context);
}

void TcpModule::on_connect(FsmContextPtr context, 
    const boost::system::error_code& error) {
  if (error) {
    on_connect_failed(context);
    cancel_time_out_timer(context);
    context->fsm()->resume(this, Mod_Failed, context);
  } else {
    send_request(context);
  }
}

ModState TcpModule::send_request(FsmContextPtr context) {
  char* req = NULL;
  uint32_t req_len;
  get_request(&req, &req_len, context);
  ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());
  boost::asio::async_write(data->client_->socket(), 
      boost::asio::buffer(req, req_len),
      boost::bind(&TcpModule::on_write_req, this, context,
        boost::asio::placeholders::error));
  return Mod_async;
}

void TcpModule::on_write_req(FsmContextPtr context, const boost::system::error_code& error) {
  if (error) {
    on_send_req_failed(context);
    context->fsm()->exit();
    context->fsm()->resume(this, Mod_Failed, context);
  } else {
    ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());

    uint32_t head_len = get_resp_head_len();
    char* head = new char[head_len];
    boost::asio::async_read(data->client_->socket(), 
        boost::asio::buffer(head, head_len),
        boost::bind(&TcpModule::on_recv_resp_head, this, context, 
          boost::shared_array<char>(head), head_len,
          boost::asio::placeholders::error));
  }
}

void TcpModule::on_recv_resp_head(FsmContextPtr context, 
    boost::shared_array<char> head, 
    uint32_t len, 
    const boost::system::error_code& error) {
  if (error) {
    on_recv_resp_failed(context);
    cancel_time_out_timer(context);
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  } 
  uint32_t body_len = get_resp_body_len(head.get(), len, context);
  // check head failed
  if (body_len == -1) {
    on_read_head_failed(context);
    cancel_time_out_timer(context);
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  }

  ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());

  char* body = new char[body_len];
  boost::asio::async_read(data->client_->socket(),
      boost::asio::buffer(body, body_len),
      boost::bind(&TcpModule::on_recv_resp_body, this, context,
        boost::shared_array<char>(body), body_len,
        boost::asio::placeholders::error));
  
}

void TcpModule::on_recv_resp_body(FsmContextPtr context, 
    boost::shared_array<char> body, 
    uint32_t len, 
    const boost::system::error_code& error) {
  cancel_time_out_timer(context);
  if (error) {
    on_recv_resp_failed(context);
    context->fsm()->resume(this, Mod_Failed, context);
    return;
  }

  // recollect tcp client
  ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());
  client_pool_.recollect(data->client_);

  bool ret = on_recv_resp_impl(body, len, context);
  ModState state = (ret ? Mod_Succeed : Mod_Failed);

  context->fsm()->resume(this, state, context);
}

void TcpModule::cancel_time_out_timer(FsmContextPtr context) {
  boost::asio::deadline_timer& timer = static_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.cancel();
}

void TcpModule::on_handle_time_out(FsmContextPtr context) {
  ModuleData* data = static_cast<ModuleData*>(context->get_module_data(name()).get());
  data->client_->socket().cancel();
  on_handle_time_out_impl(context);
  context->fsm()->resume(this, Mod_Timeout, context);
}


}
