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
#include "ReqModuleData.h"
#include "GenFsReqModuleData.h"
#include "FsForkModuleData.h"
#include "FsModuleData.h"
#include "PsForkModule2Data.h"
#include "PsBackfillModule2Data.h"
#include "PsModule2Data.h"
#include "PsJoinModule2Data.h"
#include "FsJoinModuleData.h"
#include "GenPsReqModuleData.h"
#include "PsForkModuleData.h"
#include "PsModuleData.h"
#include "PsBackfillModuleData.h"
#include "PsJoinModuleData.h"
#include "RespModuleData.h"
#include "IfRandModuleData.h"
#include "RandRespModuleData.h"
#include "EmptyRespModuleData.h"
#include "EndModuleData.h"

#define DEFINE_MODULE_DATA(NNNNModuleData, nnnn_mod_data) \
public: \
  NNNNModuleData* get##NNNNModuleData() { \
    return nnnn_mod_data##_.get(); \
  } \
private: \
  boost::shared_ptr<NNNNModuleData> nnnn_mod_data##_;

#define NEW_MODULE_DATA(NNNNModuleData, nnnn_mod_data) nnnn_mod_data##_.reset(new NNNNModuleData(data_))

using namespace zerver;

class FsmData : public IFsmData {
  public:
    FsmData() : IFsmData() {
      data_ = new Data();
      NEW_MODULE_DATA(ReqModuleData, req_mod_data);
      NEW_MODULE_DATA(GenFsReqModuleData, gen_fs_req_mod_data);
      NEW_MODULE_DATA(FsForkModuleData, fs_fork_mod_data);
      NEW_MODULE_DATA(FsModuleData, fs_mod_data);
      NEW_MODULE_DATA(PsForkModule2Data, ps_fork_mod2_data);
      NEW_MODULE_DATA(PsModule2Data, ps_mod2_data);
      NEW_MODULE_DATA(PsBackfillModule2Data, ps_backfill_mod2_data);
      NEW_MODULE_DATA(PsJoinModule2Data, ps_join_mod2_data);
      NEW_MODULE_DATA(FsJoinModuleData, fs_join_mod_data);
      NEW_MODULE_DATA(GenPsReqModuleData, gen_ps_req_mod_data);
      NEW_MODULE_DATA(PsForkModuleData, ps_fork_mod_data);
      NEW_MODULE_DATA(PsModuleData, ps_mod_data);
      NEW_MODULE_DATA(PsBackfillModuleData, ps_backfill_mod_data);
      NEW_MODULE_DATA(PsJoinModuleData, ps_join_mod_data);
      NEW_MODULE_DATA(RespModuleData, resp_mod_data);
      NEW_MODULE_DATA(RandRespModuleData, rand_resp_mod_data);
      NEW_MODULE_DATA(IfRandModuleData, if_rand_mod_data);
      NEW_MODULE_DATA(EmptyRespModuleData, empty_resp_mod_data);
      NEW_MODULE_DATA(EndModuleData, end_mod_data);
    }

    virtual ~FsmData() {
      delete data_;
    }

    virtual void reset() {
      data_->reset();
    }

    DEFINE_MODULE_DATA(ReqModuleData, req_mod_data)
    DEFINE_MODULE_DATA(GenFsReqModuleData, gen_fs_req_mod_data)
    DEFINE_MODULE_DATA(FsForkModuleData, fs_fork_mod_data)
    DEFINE_MODULE_DATA(FsModuleData, fs_mod_data)
    DEFINE_MODULE_DATA(PsForkModule2Data, ps_fork_mod2_data)
    DEFINE_MODULE_DATA(PsModule2Data, ps_mod2_data)
    DEFINE_MODULE_DATA(PsBackfillModule2Data, ps_backfill_mod2_data)
    DEFINE_MODULE_DATA(PsJoinModule2Data, ps_join_mod2_data)
    DEFINE_MODULE_DATA(FsJoinModuleData, fs_join_mod_data)
    DEFINE_MODULE_DATA(GenPsReqModuleData, gen_ps_req_mod_data)
    DEFINE_MODULE_DATA(PsForkModuleData, ps_fork_mod_data)
    DEFINE_MODULE_DATA(PsModuleData, ps_mod_data)
    DEFINE_MODULE_DATA(PsBackfillModuleData, ps_backfill_mod_data)
    DEFINE_MODULE_DATA(PsJoinModuleData, ps_join_mod_data)
    DEFINE_MODULE_DATA(RespModuleData, resp_mod_data)
    DEFINE_MODULE_DATA(RandRespModuleData, rand_resp_mod_data)
    DEFINE_MODULE_DATA(IfRandModuleData, if_rand_mod_data)
    DEFINE_MODULE_DATA(EmptyRespModuleData, empty_resp_mod_data)
    DEFINE_MODULE_DATA(EndModuleData, end_mod_data)
  private:
    Data* data_;
};

#endif
