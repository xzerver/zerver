#ifndef __ZERVER_TCP_CLIENT_H__
#define __ZERVER_TCP_CLIENT_H__

namespace zerver {
class TcpClient {
  public:
    tcp::socket& socket() { return socket_; }


  private:
    tcp::socket socket_;
}
}
#endif
