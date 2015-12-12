#ifndef __FSM_DATA_H__
#define __FSM_DATA_H__


//#include "../framework/data.h"
#include "KaraModuleData.h"
#include "GuruModuleData.h"


#define DEFINE_MODULE_DATA(NNNNModuleData, nnnn_mod_data) \
public: \
  NNNNModuleData* get##NNNNModuleData() { \
    if (nnnn_mod_data##_.get() == NULL) { \
      nnnn_mod_data##_.reset(new NNNNModuleData(data_)); \
    } \
    return nnnn_mod_data##_.get(); \
  } \
private: \
  boost::shared_ptr<NNNNModuleData> nnnn_mod_data##_;


using namespace zerver;

class FsmData : public IFsmData{
  public:
    FsmData() : IFsmData() {
      data_ = new Data();
    }

    virtual ~FsmData() {
      delete data_;
    }

    DEFINE_MODULE_DATA(KaraModuleData, kara_mod_data)
    DEFINE_MODULE_DATA(GuruModuleData, guru_mod_data)

  private:
    Data* data_;
};

#endif
