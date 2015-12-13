// Copyright (C) 2014, 2015 Fanxi Zeng
// Contact: zengfancy@126.com 
// 
// This file is part of xzerver.
// 
// xzerver is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// xzerver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with xzerver. If not, see <http://www.gnu.org/licenses/>.
//
#ifndef __FSM_DATA_H__
#define __FSM_DATA_H__


#include "../framework/data.h"
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
