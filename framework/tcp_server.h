#ifndef __ZERVER_TCP_SERVER_H__
#define __ZERVER_TCP_SERVER_H__

#include "fsm.h"

namespace zerver {

class TcpConnection {
  public:
    typedef boost::shared_ptr<TcpConnection> pointer;

    static pointer create_connection(boost::asio::io_service& io_service) {
    }

    tcp::socket& socket() { return socket_; }
  private:
    tcp::socket socket_;
};

class TcpServer {
  public:
    TcpServer(int num_threads, int port) : port_(port), 
    num_threads_(num_threads),
    acceptor_(io_service_) {
    }

    void run();
  private:
    void start_accept();
    void on_accepted(TcpConnection::pointer conn, const boost::system::error_code& error);
    boost::asio::io_service io_service_;
    tcp::acceptor acceptor_;
    int port_;
    int num_threads_;
};

}

#endif
