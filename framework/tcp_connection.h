#ifndef __ZERVER_TCP_CONNECTION_H__
#define __ZERVER_TCP_CONNECTION_H__

using boost::asio::ip::tcp;

namespace zerver {

class TcpConnection {
  public:
    TcpConnection(boost::asio::io_service& io_service) : 
      socket_(io_service),
      io_service_(io_service) {
    }

    static TcpConnectionPtr create_connection(boost::asio::io_service& io_service) {
      return TcpConnectionPtr(new TcpConnection(io_service));
    }

    tcp::socket& socket() { return socket_; }
    boost::asio::io_service& io_service() { return io_service_; }
  private:
    tcp::socket socket_;
    boost::asio::io_service& io_service_;
};

}

#endif
