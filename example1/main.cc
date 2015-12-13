#include "DataFactory.h"
#include "../framework/tcp_server.h"

using namespace zerver;

int main(int argc, char** argv) {
  FsmContext::register_fsm_data_factory(new DataFactory());

  TcpServer server(16, 30001);
  server.run();
  return 0;
}
