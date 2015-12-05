#include "tcp_server.h"

namespace zerver {

void TcpServer::run() {
  std::vector<boost::thread*> tvec;
  for (int i = 0; i < num_threads_; i++) {
    boost::thread* t = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
    tvec.push_back(t);
  }

  start_accept();

  for (int i = 0; i < num_threads_; i++) {
    tvec[i]->join();
  }

  for (int i = 0; i < num_threads_; i++) {
    delete tvec[i];
  }
  tvec.clear();
}

void TcpServer::start_accept() {
  TcpConnectionPtr conn = TcpConnection::create_connection(io_service_);
  acceptor_.async_accept(conn->socket(), boost::bind(&TcpServer::on_accepted, this, conn, boost::asio::placeholders::error));
}


void TcpServer::on_accepted(TcpConnectionPtr conn, const boost::system::error_code& error) {
  FsmContextPtr context = FsmContextPtr(new FsmContext(conn));
  context->fsm()->handle_request(context);

  start_accept();
}
}
