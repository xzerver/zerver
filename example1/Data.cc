#include "Data.h"

void Data::reset() {
  ad_req_ver_ = 0;
  use_feature_server_ = false;
  head_len_ = 0;
  origin_model_id_ = 0;
  model_id_ = 0;
  random_percent_ = 0;
  is_random_model_id_ = false;
  do_random_ = false;
  multi_model_ = false;
  pos_id_ = 0;
  sku_num_ = 0;
  reply_len_ = 0;
  ad_req_body_size_ = 0;
  user_pin_hash_ = 0;
  ad_req_.reset();
  ps_req_sku_nums_.clear();
  fs_req_sku_nums_.clear();
  shard_fs_num_ = 0;
  shard_fs_ = false;
  fs_req_lens_.clear();
  fs_reqs_.clear();
  fs_resp_sizes_.clear();
  fs_resp_compressed_vec_.clear();
  fs_resps_.clear();
  sub_model_id_list_.clear();
  ps_req_compressed_ = false;
  ps_reqs_.clear();
  shard_ps_num_ = 0;
  shard_ps_ = false;
  split_time_ = 0;
  fs_res_time_.clear(); 
  ps_start_time_.clear();
  ps_res_time_.clear();
  ps_req_lens_.clear();
  ps_resps_.clear();
  ps_resp_sizes_.clear();
}

