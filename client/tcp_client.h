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
#ifndef __ZERVER_TCP_CLIENT_H__
#define __ZERVER_TCP_CLIENT_H__

#include "../framework/types_def.h"
#include <boost/asio.hpp>

namespace zerver {

class TcpClient
{
public:
  TcpClient(boost::asio::io_service& io_service, int peer_id)
  :peer_id_(peer_id), socket_(io_service, boost::asio::ip::tcp::v4())
  {
    times_ = 0;
    is_closed_ = false;
    const int buffer_size = 4*65536;
    boost::system::error_code ec;
    boost::asio::ip::tcp::socket::send_buffer_size send_size (buffer_size);    
    boost::asio::ip::tcp::socket::receive_buffer_size receive_size(buffer_size);
    socket_.set_option(send_size, ec);
    socket_.set_option(receive_size, ec);
    last_touch_time_ = time(NULL);
  }

  virtual ~TcpClient()
  {
    close();
  }

  bool is_closed()
  {
    if (is_closed_) {
      return true;
    }
    return false;
  }

  bool is_error()
  {
    int rc;
    boost::system::error_code error;
    boost::asio::detail::socket_ops::state_type state;
    state = boost::asio::detail::socket_ops::user_set_non_blocking;
    rc = boost::asio::detail::socket_ops::poll_read(socket_.native_handle(), state, error);
    if(rc > 0 && !error) {
       int flags = MSG_PEEK;
       char buff[8];
       boost::asio::detail::socket_ops::buf tmp_buf;
       boost::asio::detail::socket_ops::init_buf(tmp_buf, buff, sizeof(buff));
       rc = boost::asio::detail::socket_ops::recvmsg(socket_.native_handle(), &tmp_buf, 1, flags, flags, error);
       if(rc <= 0 || error) {
         return true;
       }
    } 
    else if (rc < 0 || (error && error && error != boost::asio::error::would_block)) {
      return true;
    }
    return false;
  }

  int increase_times()
  {
    return ++times_;
  }

  void close()
  {
    if (!is_closed_) {
      boost::system::error_code err;
      socket_.close(err);
      is_closed_ = true;
    }
  }

  void on_recollect()
  {
    last_touch_time_ = time(NULL);
  }

  int get_peer_id()
  {
    return peer_id_;
  }

  bool expired(time_t now, int time)
  {
    return now - last_touch_time_ >= time; 
  }

  boost::asio::ip::tcp::socket & socket()
  {
    return socket_;
  }

  std::string remote_ip()
  {
    std::string ip;
    boost::system::error_code ec; 
    boost::asio::ip::tcp::socket::endpoint_type remote_ip(socket_.remote_endpoint(ec));
    if (!ec) {
      ip = remote_ip.address().to_v4().to_string();
    } else {
      F_LOG(WARN) << "remote_endpoint:" << ec.message();
    }
    return ip;
  }

protected:
  int times_;
  int peer_id_;
  bool is_closed_;
  time_t last_touch_time_;
  boost::asio::ip::tcp::socket socket_;
};

typedef boost::shared_ptr<TcpClient> TcpClientPtr;

}

#endif

