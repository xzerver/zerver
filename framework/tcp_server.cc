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
#include "tcp_server.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "p_log.h"
#include <boost/unordered_set.hpp>

namespace zerver {

static bool is_child = false;
static boost::unordered_set<pid_t> child_pid_;

static void kill_childs()
{
  boost::unordered_set<pid_t>::iterator it;
  
  it = child_pid_.begin();
  while(it != child_pid_.end())
  {
    kill(*it, SIGQUIT);
    it = child_pid_.erase(it);
  }
}

static void on_exit()
{
  kill_childs();
}

TcpServer::TcpServer(int port, int task_number)
:port_(port), task_number_(task_number)
{
  if (task_number_ < 1) {
    task_number_ = 1;
  }
  try {
    fsm_.reset(new Fsm());
    io_service_.reset(new boost::asio::io_service());
    signal_.reset(new boost::asio::signal_set(*io_service_));
    acceptor_.reset(new boost::asio::ip::tcp::acceptor(*io_service_, tcp::endpoint(tcp::v4(), port), true));
  }
  catch(std::exception &e){
    F_LOG(ERROR) << "Init TcpServer exception, port " << port_<< " " << e.what();
    clear();
    exit(-1);
  }
  catch(...) {
     F_LOG(ERROR) << "Init TcpServer exception. port " << port_;
     clear();
     exit(-1);
  }
  atexit(on_exit);
  start_signal_wait();
  server_mode_ = RUN_NULL;
  signal_->add(SIGCHLD);
  signal_->add(SIGQUIT);
  signal_->add(SIGTERM);
  signal_->add(SIGINT);
  //signal_.add(SIGKILL);
}

TcpServer::~TcpServer()
{
  clear();
}

void TcpServer::clear()
{
  fsm_.reset();
  signal_.reset();
  acceptor_.reset();
  io_service_.reset();
}


void TcpServer::start_signal_wait()
{
  signal_->async_wait(boost::bind(&TcpServer::handle_signal, this, _1, _2));
}

void TcpServer::handle_signal(const boost::system::error_code& error, int signal_number)
{
  int task_num = 0;
  boost::system::error_code err;
  F_LOG(WARN) << "recv signal " << signal_number;
  switch(signal_number)
  {
  case SIGCHLD:
    /* Only the parent process should check for this signal. We can determine
     whether we are in the parent by checking if the acceptor is still open.
    */
    if (acceptor_->is_open())
    {
      /* Reap completed child processes so that we don't end up with zombies.*/
      pid_t pid;
      int status = 0;
      while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
      {
        task_num++; 
        child_pid_.erase(pid);
      }
    }
    if(task_num > 0) {
      if(RUN_THREAD == server_mode_) {
        run_multi_thread(task_number_);
      } else if(RUN_PROCESS == server_mode_) {
        run_multi_process(task_num);
      }
    }
    break;
  case SIGQUIT:
  case SIGKILL:
  case SIGTERM:
  case SIGINT:
    acceptor_->close(err);
    on_child_killing(err);
    io_service_->stop();
    kill_childs();
    break;
  }
  start_signal_wait();
}

void TcpServer::run(run_mode mod)
{
  server_mode_ = mod;
  if(RUN_THREAD == mod) {
    run_multi_thread(task_number_);
  } else {
    run_multi_process(task_number_);
  }
  io_service_->run();
}

void TcpServer::run_multi_thread(int thread_num)
{
  io_service_->notify_fork(boost::asio::io_service::fork_prepare);
  pid_t pid = fork();
  if (0 == pid) { // child process.
    io_service_->notify_fork(boost::asio::io_service::fork_child);
    /* The child process is not interested in processing the SIGCHLD signal.*/
    start_signal_wait();
    run_for_child(thread_num);
    return;
  } else if(-1 != pid) {
    io_service_->notify_fork(boost::asio::io_service::fork_parent);
    child_pid_.insert(pid);
    return;
  } else {
    F_LOG(ERROR) << "fork process fail. " << strerror(errno) ;
  }
}

void TcpServer::run_for_child(int thread_num) {
  std::vector<boost::thread*> tvec;
  for (int i = 0; i < thread_num; i++) {
    boost::thread* t = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service_));
    tvec.push_back(t);
  }

  start_accept();
  on_child_running();

  for (int i = 0; i < thread_num; i++) {
    tvec[i]->join();
  }

  for (int i = 0; i < thread_num; i++) {
    delete tvec[i];
  }
  tvec.clear();
}

void TcpServer::run_multi_process(int process_num) {
  if (is_child) {
    F_LOG(WARN) << "run multi process error for child :" << process_num;
    return;
  }
  pid_t pid;
  io_service_->notify_fork(boost::asio::io_service::fork_prepare);
  for(int i = 0; i < process_num; i++) {
    pid = fork();
    if(0 == pid) { // child process.
      is_child = true;
      io_service_->notify_fork(boost::asio::io_service::fork_child);
      /* The child process is not interested in processing the SIGCHLD signal.*/
      start_signal_wait();
      start_accept();
      on_child_running();
      return;
    } else if(-1 != pid) { // parent process.
      io_service_->notify_fork(boost::asio::io_service::fork_parent);
      child_pid_.insert(pid);
    } else {
      F_LOG(ERROR) << "fork process fail. " << strerror(errno) ;
      return;
    }
  }
}

void TcpServer::start_accept() {
  TcpConnectionPtr conn = TcpConnection::create_connection(*io_service_);
  acceptor_->async_accept(conn->socket(), boost::bind(&TcpServer::on_accepted, this, conn, boost::asio::placeholders::error));
}


void TcpServer::on_accepted(TcpConnectionPtr conn, const boost::system::error_code& error) {
  FsmContextPtr context = FsmContextPtr(new FsmContext(conn, fsm_.get(), *io_service_));
  context->init();
  fsm_->handle_request(context);

  start_accept();
}

}

