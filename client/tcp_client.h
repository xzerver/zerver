#ifndef __ZERVER_TCP_CLIENT_H__
#define __ZERVER_TCP_CLIENT_H__

#include "../framework/types_def.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace zerver {
class TcpClient {
  public:
    TcpClient(boost::asio::io_service& io_service, uint32_t client_type_id) : 
      socket_(io_service),
      client_type_id_(client_type_id) {
    }

    tcp::socket& socket() { return socket_; }
    uint32_t client_type_id() { return client_type_id_; }

  private:
    tcp::socket socket_;
    uint32_t client_type_id_;
};

typedef boost::shared_ptr<TcpClient> TcpClientPtr;
}
#endif
