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
#ifndef __ZERVER_TYPE_DEF_H__
#define __ZERVER_TYPE_DEF_H__


#include <string>
#include <map>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
#include "utils/util.h"
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"
#include "thirdparty/perftools/tcmalloc.h"
#include "p_log.h"

typedef uint32_t ModState;
/*
inline std::string get_cur_time()
{
  struct  timeval tv;
  std::stringstream stream;
  gettimeofday(&tv, NULL);
  stream << tv.tv_sec <<"." << tv.tv_usec/1000 << " ";
  return stream.str();  
}

#define P_LOG(x) std::cout << "\n" << get_cur_time() << (uint32_t) pthread_self() <<  " " <<__FILE__ << ":" << __LINE__ << ":" << "[" #x "]:"
*/
namespace zerver {

class IFsmData;
class Fsm;
class FsmContext;
class Module;
class IModuleData;
class TcpConnection;

typedef boost::shared_ptr<IFsmData> FsmDataPtr;
typedef boost::shared_ptr<FsmContext> FsmContextPtr;
typedef boost::shared_ptr<IModuleData> ModuleDataPtr;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::map<std::string, Module*> ModuleMap;
typedef std::map<std::string, std::map<ModState, std::string> > ModuleTransitionMap;
}

template< typename T >
struct array_deleter
{
  void operator ()(T *p)
  { 
    tc_free(p); 
  }
};

typedef boost::shared_array<char> shared_string_array;

inline shared_string_array get_shared_string_array(size_t size)
{
  return shared_string_array((char*)tc_malloc(size), array_deleter<char>());
}

inline void reset_shared_string_array(shared_string_array &src, size_t size)
{
  src.reset((char*)tc_malloc(size), array_deleter<char>());
}

#endif
