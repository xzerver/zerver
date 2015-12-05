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
