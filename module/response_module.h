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
    void on_write_resp(IFsmData* data, 
        const boost::system::error_code& error);
};

}

#endif
