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
#include "tcp_module.h"

DEFINE_int32(wait_connect_time_out_ms, 50, "wait connect time out ms for feature/predictor server.");
DEFINE_int32(wait_reply_time_out_ms, 50, "wait reply time out ms for feature/predictor server.");
namespace zerver {

ModState TcpModule::run(FsmContextPtr context, ModState last_mod_state) {
  int client_type_id = get_client_type_id(context);
  if (-1 == client_type_id) {
    return Mod_Failed;
  }
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);
  data->client_ = client_pool_.get_client(client_type_id);
  //P_DLOG() << name() <<  " client_type_id:" << client_type_id << " " << client_pool_.size(); //  << client_pool_.debug_string();
  if (!data->client_) {
    std::string ip;
    unsigned short port;
    if (!get_server_ip_port(ip, port, context)) {
      P_LOG(ERROR) << name() <<  " get server ip/port failed. client_type_id=" << client_type_id;
      return Mod_Failed;
    }
    try {
      //P_DLOG() << "get a ip and port, ip:" << ip << ", port:" << port;
      uint32_t ms = get_connect_time_out_ms();
      boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
      boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
      timer.expires_from_now(boost::posix_time::milliseconds(ms));
      timer.async_wait(boost::bind(&TcpModule::on_handle_connect_time_out, this, context, ip, port, boost::asio::placeholders::error));

      data->client_ = TcpClientPtr(new TcpClient(context->conn()->io_service(), client_type_id));
      //P_DLOG() << "connect " << ip << ":" << port;
      data->client_->socket().async_connect(endpoint, 
        boost::bind(&TcpModule::on_connect, this, context,
          ip, port,
          boost::asio::placeholders::error));
      data->socket_event_ = Socket_Event_Opened;
    }
    catch(std::exception &e){
      P_LOG(ERROR) << "Exception:"  << e.what();
      return Mod_Failed;
    }
    catch(...) {
      P_LOG(ERROR) << "unknown exception.";
      return Mod_Failed;
    }
    return Mod_async;
  }

  uint32_t ms = get_time_out_ms();
  boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  timer.expires_from_now(boost::posix_time::milliseconds(ms));
  timer.async_wait(boost::bind(&TcpModule::on_handle_time_out, this, context, boost::asio::placeholders::error));

  return send_request(context);
}

void TcpModule::on_connect(FsmContextPtr context, 
    std::string ip, uint16_t port,
    const boost::system::error_code& error) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);
  if (data->socket_event_ != Socket_Event_Opened) {
    P_LOG(WARN) << "on_connect already called!";
    return;
  }

  data->socket_event_ = Socket_Event_Closed;
  if (error) {
    if (error == boost::asio::error::operation_aborted) {
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(WARN) << name() << ":on_connect_failed," << ip << ":" << port << " message:" << error.message();
      on_connect_failed(context, ip, port);
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {
    if (!cancel_time_out_timer(context)) {
      return;
    }
    //P_DLOG() << "connect " << ip << ":" << port << " success." ;
    uint32_t ms = get_time_out_ms();
    boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
    timer.expires_from_now(boost::posix_time::milliseconds(ms));
    timer.async_wait(boost::bind(&TcpModule::on_handle_time_out, this, context, boost::asio::placeholders::error));

    send_request(context);
  }
}

void TcpModule::on_handle_connect_time_out(FsmContextPtr context, 
    std::string ip, uint16_t port,
    const boost::system::error_code & ec) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);
  if (ec) {
    // the timer be canceled
    if (ec != boost::asio::error::operation_aborted) {
      P_LOG(ERROR) << "error occured:" << ec.message();
    }
    return;
  }
  P_LOG(WARN) << name() << " connect time out:" << get_connect_time_out_ms() << "ms. IP:" << ip << ":" << port;
  if (NULL != data && data->client_) {
    boost::system::error_code err;
    data->client_->socket().cancel(err);
    if (err) {
      P_LOG(ERROR) << "socket cancel error occured:" << ec.message();
    }
  } else {
    P_LOG(ERROR) << "Shouldn't have to go here.";
  }
  on_connect_failed(context, ip, port);
}

ModState TcpModule::send_request(FsmContextPtr context) {
  uint32_t req_len;
  shared_string_array req = get_request(&req_len, context);
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  boost::asio::async_write(data->client_->socket(), 
      boost::asio::buffer(req.get(), req_len),
      boost::bind(&TcpModule::on_write_req, this, context,
        req, req_len, // pass the req to avoid the memory release of req, for the sake of buffer's dependency on it
        boost::asio::placeholders::error));
  data->socket_event_ = Socket_Event_Opened;
  return Mod_async;
}

void TcpModule::on_write_req(FsmContextPtr context, 
    shared_string_array req, uint32_t req_len,
    const boost::system::error_code& error) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);
  if (data->socket_event_ != Socket_Event_Opened) {
    P_LOG(WARN) << "on_write_req already called!";
    return;
  }
  data->socket_event_ = Socket_Event_Closed;
  if (error) {
    if (error == boost::asio::error::operation_aborted) {
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(WARN) << name() << ":on_send_req_failed";
      on_send_req_failed(context);
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {

    uint32_t head_len = get_resp_head_len();
    shared_string_array shared_head = get_shared_string_array(head_len);
    char* head = shared_head.get();
    boost::asio::async_read(data->client_->socket(), 
        boost::asio::buffer(head, head_len),
        boost::bind(&TcpModule::on_recv_resp_head, this, context, 
          shared_head, head_len,
          boost::asio::placeholders::error));
    data->socket_event_ = Socket_Event_Opened;
  }
}

void TcpModule::on_recv_resp_head(FsmContextPtr context, 
    shared_string_array head, 
    uint32_t len, 
    const boost::system::error_code& error) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);
  if (data->socket_event_ != Socket_Event_Opened) {
    P_LOG(WARN) << "on_recv_resp_head already called!";
    return;
  }
  data->socket_event_ = Socket_Event_Closed;
  if (error || !on_read_head_impl(head, len, context)) {
    if (error == boost::asio::error::operation_aborted) {
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(WARN) << name() << " remote_ip:" << data->client_->remote_ip() << " on_read_head_failed " << error.message();
      data->client_->close();
      on_read_head_failed(context);
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {
    uint32_t body_len = get_resp_body_len(context);

    shared_string_array shared_body = get_shared_string_array(body_len);
    char* body = shared_body.get();
    boost::asio::async_read(data->client_->socket(),
        boost::asio::buffer(body, body_len),
        boost::bind(&TcpModule::on_recv_resp_body, this, context,
          shared_body, body_len,
          boost::asio::placeholders::error));
    data->socket_event_ = Socket_Event_Opened;
  }
}

void TcpModule::on_recv_resp_body(FsmContextPtr context, 
    shared_string_array body, 
    uint32_t len, 
    const boost::system::error_code& error) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);
  if (data->socket_event_ != Socket_Event_Opened) {
    P_LOG(WARN) << "on_recv_resp_body already called!";
    return;
  }
  data->socket_event_ = Socket_Event_Closed;
  if (error) {
    if (error == boost::asio::error::operation_aborted) {
      context->fsm()->resume(this, Mod_Timeout, context);
    } else {
      if (!cancel_time_out_timer(context)) {
        return;
      }
      P_LOG(WARN) << name() << ":on_read_body_failed";
      on_read_body_failed(context);
      context->fsm()->resume(this, Mod_Failed, context);
    }
  } else {
    if (!cancel_time_out_timer(context)) {
      return;
    }

    // recollect tcp client
    client_pool_.recollect(data->client_);
    data->client_.reset();

    bool ret = on_read_body_impl(body, len, context);
    ModState state = (ret ? Mod_Succeed : Mod_Failed);

    context->fsm()->resume(this, state, context);
  }
}

bool TcpModule::cancel_time_out_timer(FsmContextPtr context) {
  boost::asio::deadline_timer& timer = dynamic_cast<ModuleData*>(context->get_module_data(name()).get())->timer_;
  boost::system::error_code err;
  timer.cancel(err);
  if (err) {
    P_LOG(ERROR) << "ERROR: Error when cancel the time out timer: err message:" << err.message();
    return false;
  }

  return true;
}

void TcpModule::on_handle_time_out(FsmContextPtr context,
    const boost::system::error_code & ec) {
  ModuleData* data = dynamic_cast<ModuleData*>(context->get_module_data(name()).get());
  ModuleData::Lock lock(data);

  if (ec) {
    // the timer be canceled
    if (ec != boost::asio::error::operation_aborted) {
      P_LOG(ERROR) << "error occured:" << ec.message();
    }
    return;
  }

  if (NULL != data && data->client_) {
    std::string ip = data->client_->remote_ip();
    boost::system::error_code err;
    data->client_->socket().cancel(err);
    if (err) {
      P_LOG(ERROR) << name() <<  " time out:" << get_time_out_ms() << "ms, socket cancel error occured:" << ec.message();
    } else {
      P_LOG(ERROR) << name() << " time out:" << get_time_out_ms() << "ms, ip:" << ip;
    }
  } else {
    P_LOG(ERROR) << name() << " time out:" << get_time_out_ms() << "ms, Shouldn't have to go here.";
  }
  on_handle_time_out_impl(context);
}

uint32_t TcpModule::get_connect_time_out_ms()
{
  return FLAGS_wait_connect_time_out_ms;
}

uint32_t TcpModule::get_time_out_ms()
{
  return FLAGS_wait_reply_time_out_ms;
}

}

