


ModState TcpModule::run(IFsmData* data) {
  TcpClient* client = get_client();
  Request* req = get_request();
  client->send_req(req, on_recv_resp);

  return Mod_async;
}


void TcpModule::on_recv_resp(IFsmData* data) {
  bool ret = on_recv_resp_impl(data);

  data->fsm()->resume(this, ret);
}

void TcpModule::on_recv_time_out(IFsmData* data) {
  on_recv_time_out_impl(data);
  data->fsm()->resume(this, Mod_timeout);
}

Request* TcpModule::get_request(IFsmData* data) {
  return get_request_impl(data);
}
