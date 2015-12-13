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

#ifndef __MODULE_DATA_DEFINES_H__
#define __MODULE_DATA_DEFINES_H__

#define BEGIN_MODULE_DATA(KaraModuleData, KaraModule) \
class KaraModuleData { \
  public: \
    KaraModuleData(Data* data) : data_(data) {} \
    friend class KaraModule; \
  private: \
    Data* data_;


#define DEFINE_PRIM_VAR_R(type, v1) \
private: \
  type get_##v1() { return data_->get_##v1(); } 

#define DEFINE_PRIM_VAR_W(type, v1) \
private: \
  void set_##v1(type v1) { data_->set_##v1(v1); } 

#define DEFINE_PRIM_VAR_RW(type, v1) \
private: \
  void set_##v1(type v1) { data_->set_##v1(v1); } \
  type get_##v1() { return data_->get_##v1(); } 

#define DEFINE_OBJ_VAR_R(type, o1) \
private: \
  const type& get_const_##o1() { return data_->get_const_##o1(); } 

#define DEFINE_OBJ_VAR_W(type, o1) \
private: \
  type& get_mutable_##o1() { return data_->get_mutable_##o1(); } 

#define DEFINE_OBJ_VAR_RW(type, o1) \
private: \
  const type& get_const_##o1() { return data_->get_const_##o1(); } \
  type& get_mutable_##o1() { return data_->get_mutable_##o1(); } 

#define END_MODULE_DATA() \
};

#endif
