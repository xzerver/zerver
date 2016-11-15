#ifndef _P_LOG_H_
#define _P_LOG_H_

#include <iostream>

#define P_DLOG()  std::cout << "DEBUG PID:" << getpid() << " UUID:[" << context->UUID() << "]: "
#define P_LOG(LV) std::cout << "PID:" << getpid() << " UUID:[" << context->UUID() << "]: "
#define F_DLOG()  std::cout << "DEBUG PID:" << getpid() << " "
#define F_LOG(LV) std::cout << "PID:" << getpid() << " "

typedef std::string (*load_info_func) ();

#endif // _P_LOG_H_

