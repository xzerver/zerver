#include "tcp_client_pool.h"


namespace zerver {

TcpClientPtr TcpClientPool::get_client(uint32_t client_type_id) {
  _lock();
  ClientMap::iterator it = client_map_.find(client_type_id);
  if (it != client_map_.end()) {
    if (!it->second.empty()) {
      TcpClientPtr client = it->second.back();
      // pop from the back
      it->second.pop_back();
      return client;
    } else {
      return TcpClientPtr();
    }
  } else {
    return TcpClientPtr();
  }

  _unlock();
}

void TcpClientPool::recollect(TcpClientPtr client) {
  _lock();

  uint32_t client_type_id = client->client_type_id();
  ClientMap::iterator it = client_map_.find(client_type_id);
  if (it != client_map_.end()) {
    // push from the front
    it->second.push_front(client);
  } else {
    std::deque<TcpClientPtr> q;
    q.push_front(client);
    client_map_[client_type_id] = q;
  }

  _unlock();
}

}
