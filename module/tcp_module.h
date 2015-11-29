


class TcpCliModule : public Module {
  TcpCliModule(TcpClientPool* pool) : client_pool_(pool) {
  }

  bool is_async() { return false; }
  virtual ModState run(IFsmData* data);

  private:
  TcpClient* get_client(IFsmData* data);
  Request* get_request(IFsmData* data);
  void on_recv_resp(IFsmData* data);
  void on_recv_time_out(IFsmData* data);

  virtual Request* get_request_impl(IFsmData* data);
  virtual void on_send_req_failed(IFsmData* data);
  virtual void on_recv_resp_impl(IFsmData* data);
  virtual void on_recv_time_out_impl(IFsmData* data);

  TcpClientPool* client_pool_;
};
