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
#ifndef __FSM_DATA_DEFINES_H__
#define __FSM_DATA_DEFINES_H__


#define DEFINE_MODULE_DATA(KaraModuleData, kara_mod_data) \
public: \
  KaraModuleData* get_##KaraModuleData() { \
    if (kara_mod_data##_.is_null()) { \
      kara_mod_data##_.reset(new KaraModuleData(data_)); \
    } \
    return kara_mod_data##_.get(); \
  } \
private: \
  boost::shared_ptr<KaraModuleData> kara_mod_data##_;


#define DEFINE_PRIM_VAR_R(type, v1) \
public: \
  type get_##v1() { return v1##_; } \
private: \
  type v1##_;

#define DEFINE_PRIM_VAR_W(type, v1) \
public: \
  type set_##v1(type v1) { v1##_ = v1; } \
private: \
  type v1##_;

#define DEFINE_PRIM_VAR_RW(type, v1) \
public: \
  type get_##v1() { return v1##_; } \
  type set_##v1(type v1) { v1##_ = v1; } \
private: \
  type v1##_;

#define DEFINE_OBJ_VAR_R(type, o1) \
public: \
  const type& get_const_##o1() { return o1##_; } \
private: \
  type o1##_;

#define DEFINE_OBJ_VAR_W(type, o1) \
public: \
  type& get_mutable_##o1() { return o1##_; }  \
private: \
  type o1##_;

#define DEFINE_OBJ_VAR_RW(type, o1) \
public: \
  const type& get_const_##o1() { return o1##_; } \
  type& get_mutable_##o1() { return o1##_; }  \
private: \
  type o1##_;

#endif
