#ifndef __ZERVER_REQUEST_MODULE_H__
#define __ZERVER_REQUEST_MODULE_H__

#include "../framework/module.h"
#include <boost/shared_array.hpp>

namespace zerver {

class RequestModule : public Module {
  public:
    class ModuleData : public IModuleData {
      public:
        boost::asio::deadline_timer timer_;
    };

    RequestModule(const std::string& name) : Module(name) {
    }

    virtual bool is_async() { return true; }
    virtual ModState run(FsmContextPtr context);

  protected:
    virtual uint32_t get_head_len() { return 256; }
    // return -1 if head illegal
    virtual uint32_t get_body_len(const char* head, uint32_t len, FsmContextPtr context) = 0;
    virtual uint32_t get_read_head_time_out_ms() { return 30 * 1000; }
    virtual uint32_t get_read_body_time_out_ms() { return 30; }

    virtual void on_read_head_failed(FsmContextPtr context) {}
    virtual void on_read_body_failed(FsmContextPtr context) {}
    virtual void on_read_head_time_out(FsmContextPtr context) {}
    virtual void on_read_body_time_out(FsmContextPtr context) {}

    virtual bool on_read_body_impl(boost::shared_array<char> body, 
        uint32_t len, FsmContextPtr context) = 0;
  
  private:
    void on_read_head(FsmContextPtr context,
        boost::shared_array<char> head, 
        uint32_t len, 
        const boost::system::error_code& ec);

    void on_read_body(FsmContextPtr context,
        boost::shared_array<char> body, 
        uint32_t len, 
        const boost::system::error_code& ec);

    void on_read_time_out(uint32_t flag, FsmContextPtr context);
};


}


#endif
