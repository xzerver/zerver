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
  TcpConnection::pointer conn = TcpConnection::create_connection(io_service_);

  acceptor_.async_accept(conn->socket(), boost::bind(&TcpServer::on_accepted, this, conn, boost::asio::placeholders::error));
}


void TcpServer::on_accepted(TcpConnection::pointer conn, const boost::system::error_code& error) {
  Fsm::pointer fsm = Fsm::create_fsm(conn->socket());
  fsm->handle_request();

  start_accept();
}
}
