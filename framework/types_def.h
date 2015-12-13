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
#include <boost/shared_ptr.hpp>

typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef uint32_t ModState;

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

#endif
