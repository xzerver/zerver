#ifndef __ZERVER_RESPONSE_MODULE_H__
#define __ZERVER_RESPONSE_MODULE_H__

#include "../framework/module.h"

namespace zerver {
class ResponseModule : public Module {
  public:
    virtual bool is_async() { return true; }
    virtual ModState run(IFsmData* data);

  protected:
    virtual void get_resp(char** resp, uint32_t* resp_len) = 0;
    virtual void on_write_resp_failed() {}
    virtual void on_write_resp_time_out() {}
    virtual uint32_t get_write_resp_time_out_ms() { return 30; }
    boost::asio::deadline_timer timer_;

  private:
    void on_write_resp(IFsmData* data, 
        const boost::system::error_code& error);
    void on_write_time_out(IFsmData* data);
};

}

#endif
