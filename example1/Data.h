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

#include "CustomDataStruct.h"

#define DEFINE_PRIM_VAR(type, v1) \
public: \
  type get_##v1() { return v1##_; } \
  type set_##v1(type v1) { v1##_ = v1; } \
private: \
  type v1##_;

#define DEFINE_OBJ_VAR(type, o1) \
public: \
  const type& get_const_##o1() { return o1##_; } \
  type& get_mutable_##o1() { return o1##_; }  \
private: \
  type o1##_;


class Data {
  public:
    DEFINE_PRIM_VAR(int, id)
    DEFINE_PRIM_VAR(short, school)
    DEFINE_PRIM_VAR(float, price)
    DEFINE_OBJ_VAR(std::string, name)
    DEFINE_OBJ_VAR(CustomData1, data1)
    DEFINE_OBJ_VAR(CustomData2, data2)
};


#endif
