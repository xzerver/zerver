#ifndef __ZERVER_TCP_CLIENT_POOL_H__
#define __ZERVER_TCP_CLIENT_POOL_H__

#include "tcp_client.h"
#include <deque>

namespace zerver {

class TcpClientPool {
  public:
    TcpClientPtr get_client(uint32_t client_type_id);
    void recollect(TcpClientPtr client);

  protected:
    void _lock();
    void _unlock();

  protected:
    typedef std::map<uint32_t, std::deque<TcpClientPtr> > ClientMap;
    ClientMap client_map_;
};
}


#endif
