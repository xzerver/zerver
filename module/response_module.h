#ifndef __ZERVER_RESPONSE_MODULE_H__
#define __ZERVER_RESPONSE_MODULE_H__

#include "../framework/module.h"

namespace zerver {
class ResponseModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        boost::asio::deadline_timer timer_;
    };

    ResponseModule(const std::string& name) : Module(name) {
    }
    virtual bool is_async() { return true; }
    virtual ModState run(FsmContextPtr context);

  protected:
    virtual void get_resp(char** resp, uint32_t* resp_len, FsmContextPtr context) = 0;
    virtual void on_write_resp_failed(FsmContextPtr context) {}
    virtual void on_write_resp_time_out(FsmContextPtr context) {}
    virtual void on_write_resp_succeed(FsmContextPtr context) {}
    virtual uint32_t get_write_resp_time_out_ms() { return 30; }

  private:
    void on_write_resp(FsmContextPtr context, 
        const boost::system::error_code& error);
    void on_write_time_out(FsmContextPtr context);
};

}

#endif
