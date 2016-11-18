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
#ifndef __EXAMPLE_DATA_H__
#define __EXAMPLE_DATA_H__

#include <vector>
#include <sys/time.h>
#include "framework/types_def.h"

#include "types_def.h"

#define DEFINE_PRIM_VAR(type, v1) \
public: \
  type get_##v1() { return v1##_; } \
  void set_##v1(type v1) { v1##_ = v1; } \
private: \
  type v1##_;

#define DEFINE_OBJ_VAR(type, o1) \
public: \
  const type& get_const_##o1() { return o1##_; } \
  type& get_mutable_##o1() { return o1##_; }  \
private: \
  type o1##_;

inline int elapsed_ms_from_now(const struct timeval & time)
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return (tv.tv_sec - time.tv_sec)*1000 + (tv.tv_usec - time.tv_usec)/1000;
}

class Data {
  public:
    void reset();

    DEFINE_PRIM_VAR(char, ad_req_ver)
    DEFINE_PRIM_VAR(bool, use_feature_server)
    DEFINE_PRIM_VAR(int, head_len)
    DEFINE_PRIM_VAR(int, origin_model_id)
    DEFINE_PRIM_VAR(int, model_id)
    DEFINE_PRIM_VAR(int, random_percent)
    DEFINE_PRIM_VAR(bool, is_random_model_id)
    DEFINE_PRIM_VAR(bool, do_random)
    DEFINE_PRIM_VAR(bool, multi_model)
    DEFINE_PRIM_VAR(int, pos_id)
    DEFINE_PRIM_VAR(int, sku_num)
    DEFINE_PRIM_VAR(int, ad_req_body_size)
    DEFINE_PRIM_VAR(int, user_pin_hash)
    DEFINE_OBJ_VAR(shared_string_array, ad_req)
    DEFINE_OBJ_VAR(std::vector<int>, fs_req_sku_nums)
    DEFINE_OBJ_VAR(std::vector<int>, ps_req_sku_nums)
    DEFINE_PRIM_VAR(int, shard_fs_num)
    DEFINE_PRIM_VAR(bool, shard_fs)
    DEFINE_PRIM_VAR(int, split_time)
    DEFINE_PRIM_VAR(int, reply_len)
    DEFINE_OBJ_VAR(std::vector<uint32_t>, fs_req_lens)
    DEFINE_OBJ_VAR(struct timeval, fs_start_time)
    DEFINE_OBJ_VAR(struct timeval, ads_start_time)
    DEFINE_OBJ_VAR(std::vector<int32_t>, fs_res_time)
    DEFINE_OBJ_VAR(std::vector<shared_string_array>, fs_reqs)
    DEFINE_OBJ_VAR(std::vector<uint32_t>, fs_resp_sizes)
    DEFINE_OBJ_VAR(std::vector<bool>, fs_resp_compressed_vec)
    DEFINE_OBJ_VAR(std::vector<shared_string_array>, fs_resps)
    DEFINE_OBJ_VAR(std::vector<int>, sub_model_id_list)
    // only used when feature server not enabled, use fs_resp_compressed_vec instead when use feature server
    DEFINE_PRIM_VAR(bool, ps_req_compressed)
    // only used when feature server not enabled,
    DEFINE_OBJ_VAR(std::vector<shared_string_array>, ps_reqs)
    // only used when feature server not enabled,
    DEFINE_PRIM_VAR(int, shard_ps_num)
    // only used when feature server not enabled,
    DEFINE_PRIM_VAR(bool, shard_ps)
    // only used when feature server not enabled,
    DEFINE_OBJ_VAR(std::vector<uint32_t>, ps_req_lens)
    DEFINE_OBJ_VAR(std::vector<struct timeval>, ps_start_time)
    DEFINE_OBJ_VAR(std::vector<int32_t>, ps_res_time)
    // both used in PsModule & PsModule2
    DEFINE_OBJ_VAR(std::vector<shared_string_array>, ps_resps)
    // both used in PsModule & PsModule2
    DEFINE_OBJ_VAR(std::vector<uint32_t>, ps_resp_sizes)

};
#endif

